# Readme for OilChecker application
Copyright 2020 Kerry Loux

See LICENSE file for license details.

To build on clean install of Raspian Buster:

1. Install openssl library:
````
$ cd <directory of your choice>
$ git clone git://git.openssl.org/openssl.git
$ cd openssl
$ ./Configure linux-armv4 no-ssl2 no-ssl3 no-comp
$ make depend
$ make
$ sudo make install
````
  
2. Install libcurl (more detail available here: https://curl.se/docs/install.html):
````
$ cd <directory of your choice>
$ sudo apt install libtool autoconf
$ git clone git://github.com/bagder/curl.git
$ cd curl
$ autoreconf -fi
$ ./configure --with-ssl
$ make
$ sudo make install
````
  
3. Build the project:
````
$ cd <directory of your choice>
$ git clone https://github.com/KerryL/oilChecker.git
$ cd oilChecker
$ git submodule init
$ git submodule update   <--- see notes about this step below in 3a
$ make
````
  
3a. I linked the submodules with ssh.  If you're not me, you'll want to add remotes using the https url.  If you are me, note that I had to do the following in order to avoid errors (due to a bug?) when cloning submodules using a password-protected ssh key:
````
$ sudo apt install keychain
$ vi ~/.bashrc
````
Add the following to the end of .bashrc:
````
keychain ~/.ssh/<file name of private key>
. ~/.keychain/$HOSTNAME-sh
````

4. Tell the system to allow use of the 1-wire temperature sensor interface:
````
$ sudo vi /boot/config.txt
````
Add the following to the end of config.txt:
````
dtoverlay=w1-gpio
````

5. Configure the system to run the application on startup:
````
  $ crontab -e
````
  If prompted, choose your preferred text editor.  Then add the following line to the bottom of the cron table:
````
  @reboot <full path to oilChecker/run.sh>
````

## Notes
I encountered a bizzare issue with DNS failing some time after the Raspberry Pi booted.  I tried many fixes, but nothing seemed to work until I found this:  https://www.raspberrypi.org/forums/viewtopic.php?t=273602.  The solution described on this page worked for me (this post:  https://www.raspberrypi.org/forums/viewtopic.php?t=273602#p1659223).
 
See notes in email subrepo's readme file about how to obtain the Client ID and Client Secret and how to enable APIs for your Google account.

When you first create your project, the application will be unpublished, which means that credentials will be for "Test Users" and will expire every 7 days.
