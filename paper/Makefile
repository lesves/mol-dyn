all: main.pdf

show: all
	open main.pdf

clean:
	rm -rf fig_1.eps fig_2.eps fig_3.eps fig_4.eps fig_5.eps fig_6.eps fig_7.eps *.pdf *.log *.aux

main.pdf: main.tex fig_1.eps fig_2.eps fig_3.eps fig_4.eps fig_5.eps fig_6.eps fig_7.eps
	pdflatex main.tex
	pdflatex main.tex

%.eps: %.py
	python3 $<

fig_6.eps: fig_6.py results/timetest-result.txt
	python3 $<

fig_7.eps: fig_7.py ../scripts/arraytest.py
	python3 $<
