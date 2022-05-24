/*
 * This file is part of libsamsung-ipc.
 *
 * Copyright (C) 2022 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include <curl/curl.h>

/* CURL documentation for curl_easy_escape[1] mentions that "This
 * function does not accept input strings longer than
 * CURL_MAX_INPUT_LENGTH (8 MB)." but CURL_MAX_INPUT_LENGTH is not
 * exported in the curl public headers (in /usr/include/curl).
 */
#define CURL_MAX_INPUT_LENGTH 8000000

int send_https(CURL *hnd, char *url, __attribute__((unused)) char *post_data)
{
	CURLcode ret;

	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION,
			 (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	ret = curl_easy_perform(hnd);

	/*
	 * TODO: HTTP(s) return codes:
	 * 200: SMS sent
	 * 400: missing parameter (user, login, message)
	 * 402: Too many SMS sent in too little time
	 * 403: Possible causes:
	 *      - Service is not enabled inside user account web interface
	 *      - Wrong login or password
	 * 500: Server error => Retry later
	 */

	return (int)ret;
}

int create_parameter(CURL *hnd,
		     char **out, const char *parameter, const char *value)
{
	char *result = NULL;
	char *encoded_value;
	size_t size = 0;
	size_t rc;

	assert(strlen(value) < CURL_MAX_INPUT_LENGTH);
	encoded_value = curl_easy_escape(hnd, value, 0);
	assert(encoded_value != NULL);

	rc = snprintf(result, size, "%s=%s", parameter, encoded_value);

	assert(rc > 0);
	size = rc;

	result = malloc(size + 1);
	if (result == NULL) {
		rc = -errno;
		printf("%s: error %zd: %s\n", __func__, rc, strerror(rc));
		return rc;
	}

	rc = snprintf(result, size + 1, "%s=%s", parameter, encoded_value);
	assert(rc == size);

	assert(rc < CURL_MAX_INPUT_LENGTH);

	*out = result;

	curl_free(encoded_value);

	return 0;
}

int send_sms_get(const char *username, const char *password,
		 const char *message)
{
	CURL *hnd;
	char *parameter = NULL;
	int rc;
	CURLU *url;
	char *url_string;

	url = curl_url();
	hnd = curl_easy_init();

	rc = curl_url_set(url, CURLUPART_URL,
			  "https://smsapi.free-mobile.fr/sendmsg", 0);
	assert(rc == 0);

	rc = create_parameter(hnd, &parameter, "user", username);
	assert(rc == 0);
	rc = curl_url_set(url, CURLUPART_QUERY, parameter, CURLU_APPENDQUERY);
	assert(rc == 0);
	free(parameter);

	rc = create_parameter(hnd, &parameter, "pass", password);
	rc = curl_url_set(url, CURLUPART_QUERY, parameter, CURLU_APPENDQUERY);
	assert(rc == 0);
	free(parameter);

	rc = create_parameter(hnd, &parameter, "msg", message);
	rc = curl_url_set(url, CURLUPART_QUERY, parameter, CURLU_APPENDQUERY);
	assert(rc == 0);
	free(parameter);

	rc = curl_url_get(url, CURLUPART_URL, &url_string, 0);
	assert(rc == 0);

	rc = send_https(hnd, url_string, NULL);

	assert(rc == CURLE_OK);

	curl_easy_cleanup(hnd);
	curl_url_cleanup(url);

	return 0;
}

void usage(char *progname)
{
	printf("Usage: %s free-mobile <username> <token> <message>\n\n",
	       progname);
	printf("Example:\n\t%s %s \"%s\" \"%s\" \"%s\"\n",
	       progname,
	       "free-mobile",
	       "12345678",
	       "1234abcdEFGH",
	       "hello world!");
}

int main(int argc, char *argv[])
{
	char *message;
	char *password;
	char *username;
	int rc;

	if (argc != 5 || strncmp("free-mobile", argv[1],
				 strlen("free-mobile"))) {
		usage(argv[0]);
		return EX_USAGE;
	}

	username = argv[2];
	password = argv[3];
	message = argv[4];

	rc = send_sms_get(username, password, message);
	if (rc == 0)
		printf("OK\n");
	return rc;
}
