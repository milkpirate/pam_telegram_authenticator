# Two-Factor Authentication - PAM + Telegram !!


This project provides two-factor authentication to linux systems that use PAM. A code is sent to
Telegram, which must be typed at the login prompt to validate the login process.

## Create BOT at Telegram

You will need two pieces of information: your ID and a BOTID.

First, create a bot at telegram. To do so, 

1. Login to your telegram account 
2. Go to Chats and find BotFather
3. Open a chat with BotFather
4. Type /newbot and follow the instructions
5. Take note of the token to access the bot via HTTP API. **one will need to use it in the user configuration file** (botkey=...)

Second, 

6. Find the bot userinfobot
7. Open a char with userinfobot#
8. Type /start 
9. Take note of your ID number **one will need to use it in the user configuration file** (id=...)

Now you have the two pieces of information to be used at 3 (User configuration file) below.

## Installation

To install:
```bash
   make
   make install
```
To uninstall:
```bash
   make uninstall
```

## Module Configuration

This module provides only the auth and session realms. It must be configured using file
`/etc/pam.d/system-auth` in archlinux systems. Find the appropriate config file in your Linux
distribution.

The contents of `/etc/pam.d/system-auth` must be similar to:

	#%PAM-1.0

	auth	  required  pam_telegram_authenticator.so \ 
                            dir=.pam_telegram_authenticator  \
                            proxy_url=https://your.proxy.url:proxy.port/proxy.page  \
                            proxy_post_string='xyz=zabumba&auth_user=%s&auth_pass=%s' \
			    cache_timeout=180 \
                            enable_safe_codes=3

	auth      required  pam_unix.so     try_first_pass nullok
	auth      optional  pam_permit.so
	auth      required  pam_env.so

	account   required  pam_unix.so
	account   optional  pam_permit.so
	account   required  pam_time.so

	password  required  pam_unix.so     try_first_pass nullok sha512 shadow
	password  optional  pam_permit.so	

	session   required  pam_limits.so
	session   required  pam_unix.so
	session	  required  pam_telegram_authenticator.so
	session   optional  pam_permit.so

Observe that you only need to configure the auth and session realms. Basically, this module must
come right before or after `pam_unix.so`. The module parameters for the auth realm are:


#### dir=	    	       	  

Directory a user must create in his home dir to be used by the pam_telegram_authenticator. The user
must have the specified directory in order to enable the Two-Factor Authentication - If 
the file `~/<DIRECTORY>/credentials` doesn't exist, the user is allowed to log in using only 
the regular password. The file "credentials" must exist inside the directory. Keep in 
mind that this directory holds sensitive information. So, set the permissions as 
informed:

- Directory     - 0700
- "credentials" - 0600.
 
#### proxy_url=	/ proxy_post_string=	  

When authentication is needed to use the Internet, inform the "proxy" URL. The parameter 
proxy_post_string represents the HTTP POST string to be used in the proxy authentication 
process. The format of the parameters are usually as follows:

```
proxy_url=https://your.proxy.url:proxy.port/proxy.page
proxy_post_string='xyz=zabumba&auth_user=%s&auth_pass=%s'
```

The '%s' strings in `proxy_post_string` is to be used as value placeholders, as in the C 
language functions like `printf()`. Example:

```
proxy_url=https://mysuperproxy.com:8888/index.php
proxy_post_string=zone=zion&userid=%s&password=%s
```
			  
#### cache_timeout=         

In order to avoid entering an authentication code for every single login attempt, the 
module parameter cache_timeout can be used. It represents the time, in seconds, a 
previously entered code is still valid. The example keeps the grace time for 3 minutes, 
avoiding sending a new code in new login attempts if a user successfully logged in.

#### enable_safe_codes=	  

If, for some reason, it is impossible to send a code, a user can provide a previously 
registered safe code. Inform the number of safe codes that can be used. The value 0 
disables the use of safe codes.

You don't need parameters for the session realm.

## User configuration file

> IMPORTANT: Don't use this module for root accounts, you can be locked out. Also, this module 
> simply ignores the configuration for the root user (and can even cause problems).

According to the example at section 1, the user must create the file
`~/.pam_telegram_authenticator/credentials` with the following content:

```
id=<YOUR CHAT ID>
botkey=<YOUR BOT KEY>
proxy_username=<YOUR PROXY USERNAME>
proxy_password=<YOUR PROXY PASSWORD>
safe_code=<FIVE DIGITNUMBER>
safe_code=<FIVE DIGIT NUMBER>
safe_code=<FIVE DIGIT NUMBER>
```

In the case the Internet is not available, the user can provide safe codes (parameter safe_code
in the user configurable file). In order to enable this, the administrator must provide the
module parameter enable_safe_codes. The provided example enables 3 safe codes for each user.
The user can choose any 5-digit number as a safe code. After a successful login with a safe code,
the corresponding entry at the user configuration file will be marked as used.

## Miscellaneous

If you want to use SSH to access a machine with this module you must set
`ChallengeResponseAuthentication` to yes at file`/etc/ssh/sshd_config`.

Because I use libcurl to make https calls, other libraries must be present. libcurl depends on
a **lot of other libs** :confused:, so here is the list extracted using
`ldd /lib64/security/pam_telegram_authenticator.so`:

	libcurl.so.4
	libnghttp2.so.14
	libidn2.so.0
	libssh2.so.1
	libpsl.so.5
	libssl.so.1.1
	libcrypto.so.1.1
	libgssapi_krb5.so.2
	libkrb5.so.3
	libk5crypto.so.3
	libcom_err.so.2
	libz.so.1
	libpthread.so.0
	libc.so.6
	libunistring.so.2
	libdl.so.2
	libkrb5support.so.0
	libkeyutils.so.1
	libresolv.so.2

