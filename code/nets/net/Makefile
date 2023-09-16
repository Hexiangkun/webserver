server:
	g++ -g /home/hxk/project/webserver/code/util/util.cpp client.cpp	\
	SocketUtil.cpp EventLoop.cpp Connection.cpp Application.cpp \
	Epoller.cpp Channel.cpp Acceptor.cpp Socket.cpp Common.cpp \
	/home/hxk/project/webserver/code/util/Buffer.cpp -o client \
	-I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/ \
	-I /home/hxk/project/webserver/code/threadpool/			\
	&& \
	g++ -g /home/hxk/project/webserver/code/util/util.cpp server.cpp  	\
	/home/hxk/project/webserver/code/util/Buffer.cpp Socket.cpp	\
	SocketUtil.cpp EventLoop.cpp Connection.cpp Application.cpp \
	Epoller.cpp Channel.cpp Acceptor.cpp Common.cpp -o server  \
	-I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/	\
	-I /home/hxk/project/webserver/code/threadpool/
clean:
	rm server && rm client
