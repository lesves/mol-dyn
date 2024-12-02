all: main.pdf

show: all
	open main.pdf

clean:
	rm -rf fig_1.eps fig_2.eps *.pdf *.log *.aux

main.pdf: main.tex fig_1.eps fig_2.eps fig_3.eps fig_4.eps
	xelatex main.tex
	xelatex main.tex

fig_1.eps: fig_1.py ../ref/parareal.py
	python3 fig_1.py

fig_2.eps: fig_2.py
	python3 fig_2.py

#fig_3.eps: fig_3.py
#	python3 fig_3.py

#fig_4.eps: fig_4.py
#	python3 fig_4.py
