server:
	g++ /home/hxk/project/webserver/code/util/util.cpp client.cpp -o client \
	-I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/ \
	-I /home/hxk/project/webserver/code/threadpool/			\
	&& \
	g++ /home/hxk/project/webserver/code/util/util.cpp server.cpp  	\
	/home/hxk/project/webserver/code/util/Buffer.cpp	\
	SocketUtil.cpp EventLoop.cpp Connection.cpp Application.cpp \
	Epoller.cpp Channel.cpp Acceptor.cpp -o server  \
	-I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/	\
	-I /home/hxk/project/webserver/code/threadpool/
clean:
	rm server && rm client
