# webCpp
A simple example of a C++ web application
I use nginx, Fastcgi,CodeBlocks, https://github.com/no1msd/mstch
Extract mstch-master in webCpp folder.
Compile mstch target in CodeBlocks editor ()
sudo apt install libfcgi-dev
sudo apt install nginx
sudo apt-get install libboost-all-dev
sudo apt install codeblocks
server { 
	listen       80 default_server;
	server_name localhost; 
	location / { 
		fastcgi_pass 127.0.0.1:9000;
		include fastcgi_params; 
		}
	}
