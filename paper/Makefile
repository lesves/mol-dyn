all: main.pdf

show: all
	open main.pdf

clean:
	rm -rf fig_1.eps fig_2.eps fig_3.eps fig_4.eps *.pdf *.log *.aux

main.pdf: main.tex fig_1.eps fig_2.eps fig_3.eps fig_4.eps fig_5.eps
	xelatex main.tex
	xelatex main.tex

%.eps: %.py
	python3 $<
