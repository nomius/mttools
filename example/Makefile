all: example.pdf

example.pdf: example.txt
	S2troff < example.txt | groff -ms - | ps2pdf - > example.pdf
