pam_telegram_authenticator.o: pam_telegram_authenticator.c
	gcc -fPIC -lcurl -fno-stack-protector -c pam_telegram_authenticator.c
	ld -lcurl --shared -o pam_telegram_authenticator.so pam_telegram_authenticator.o

install: pam_telegram_authenticator.o
	install pam_telegram_authenticator.so /lib/security/pam_telegram_authenticator.so

uninstall:
	rm -f /lib/security/pam_telegram_authenticator.so
	@echo -e "\n\n      Remove any entry related to this module in /etc/pam.d/ files,\n      otherwise you're not going to be able to login.\n\n"
debug:
	gcc -E -fPIC -lcurl -fno-stack-protector -c pam_telegram_authenticator.c
clean:
	rm -rf *.o
