server:
	g++ /home/hxk/project/webserver/code/util/util.cpp client.cpp -o client -I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/ && \
	g++ /home/hxk/project/webserver/code/util/util.cpp server.cpp  SocketUtil.cpp EventLoop.cpp  Application.cpp Epoller.cpp Channel.cpp -o server  -I /home/hxk/project/webserver/ -I /home/hxk/project/webserver/code/net/
clean:
	rm server && rm client
