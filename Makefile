.PHONY: clean

out: calc case_all
	./calc < case_all > out

case_add: casegen 100 add
	./casegen add 100 > case_add
case_sub: casegen 100 sub
	./casegen sub 100 > case_sub
case_mul: casegen 100 mul
	./casegen mul 100 > case_mul
case_div: casegen 100 div
	./casegen div 100 > case_div
case_all: case_add case_sub case_mul case_div
	cat case_add case_sub case_mul case_div > case_all

clean:
	rm -f out calc casegen case_*
