test:
	g++ main.cpp libllhttp.a -o main -I /home/hxk/project/webserver/ \
	&&	\
	g++ /home/hxk/project/webserver/code/net/http/HttpParser.cpp	\
	libllhttp.a	\
	test_httppraser.cpp  -o test_httppraser \
	-I /home/hxk/project/webserver/ -lgtest	\
	&& 	\
	g++ /home/hxk/project/webserver/code/net/http/HttpHelper.cpp	\
	test_httphelper.cpp  -o test_httphelper -I /home/hxk/project/webserver/ -lgtest
clean:
	rm test_httppraser && rm main