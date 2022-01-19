# webCpp
A simple example of a C++ web application<br>
I use nginx, Fastcgi,CodeBlocks, https://github.com/no1msd/mstch<br>
Extract mstch-master in webCpp folder.<br>
Compile mstch target in CodeBlocks editor<br>
sudo apt install libfcgi-dev<br>
sudo apt install nginx<br>
sudo apt-get install libboost-all-dev<br>
sudo apt install codeblocks<br>
server { <br>
	listen       80 default_server;<br>
	server_name localhost; <br>
	location / { <br>
		fastcgi_pass 127.0.0.1:900<br>0;
		include fastcgi_params; <br>
		}<br>
	}<br>
