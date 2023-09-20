libllhttp.a: http.o llhttp.o api.o
	ar rcs $@ $^	\
	&&	\
	rm $^

http.o: http.c
	cc $^ -c -o $@


llhttp.o: llhttp.c
	cc $^ -c -o $@

api.o:api.c
	cc $^ -c -o $@

