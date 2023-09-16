server:
	g++ Common.cpp client.cpp Socket.cpp InetAddress.cpp -o client \
	-I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/ \
	-I /home/hxk/project/webserver/code/threadpool/			\
	&& \
	g++ Common.cpp server.cpp InetAddress.cpp Socket.cpp	\
	/home/hxk/project/webserver/code/util/Buffer.cpp	\
	SocketUtil.cpp EventLoop.cpp Connection.cpp Application.cpp \
	Epoller.cpp Channel.cpp Acceptor.cpp -o server  \
	-I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/	\
	-I /home/hxk/project/webserver/code/threadpool/
clean:
	rm server && rm client
