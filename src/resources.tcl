##
## Tcl Resources for XSchem
##

##
## Toolbar buttons
##
## Original artwork derived from http://www.entypo.com/
## Colours inverted, and then converted from SVG to base64-encoded GIF
##

image create photo img_filenew -data "
R0lGODlhGAAYAPUgAAAAAAQEBAgICAkJCQsLCxISEhsbG0BAQEFBQV5eXl9fX2BgYGFhYWlpaWpq
amtra3h4eJOTk5aWlp6enp+fn7+/v8HBwcTExMXFxdvb2+Pj4+Tk5O/v7/n5+fr6+v39/f///wAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAAAAAAALAAAAAAYABgAAAbSQJDw
4+kYj0ik5yNsgi4KxGFKrU4RCoxTOBEAvuBwWDDZgiQAQsLRaLvbjgQBIDGjDRuzc2Og2wF4IBwa
G4UbGhwgfH5bd3kQBQaSBgUQin11jYB5DWIADZeMTo4gFhESqBIRFqGZo5t6TYuuTaQZFRa5FhUZ
rX+BDwABwwEAD76awMLExsivgbe6u72zv3mxzrWwpqmqrNXJnJ6g4M+PkZOV2UKkg4aHieXagdjy
7Glrb29xc7RCFAM8CQQwgIIZDAukWFmIgIGWLUSSSDyypEkQADs=
"

image create photo img_fileopen -data "
R0lGODlhGAAYAPYAAAAAAAEBAQMDAwQEBAUFBQcHBwgICAkJCQsLCw0NDRISEhMTExcXFxkZGRwc
HB0dHR4eHiEhISYmJisrKywsLDAwMDExMTIyMjY2Njc3Nzk5OUREREVFRUZGRk5OTlRUVFlZWVpa
Wl1dXV5eXmBgYGhoaGlpaW1tbW5ubm9vb3BwcHl5eXp6en19fYGBgYKCgouLi5OTk5SUlJWVlZ2d
naenp6ioqKmpqbW1tbq6ury8vL29vcDAwMHBwcLCwsTExMbGxsfHx8nJydDQ0NHR0dLS0tnZ2dra
2uDg4Ojo6Ovr6+zs7PDw8Pf39/j4+Pn5+fz8/P39/f7+/v///wAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
BAAAAAAALAAAAAAYABgAAAfQgFOCg4SFhoeIiYU8LSUmJicwQoqFUisIAJkADSM4S1GUUkwimpoH
GjE5PT49ra0/R05TUkgfpZoBAgO7vLsIJEZSUUQeAATHyAS9ywEZNVBTTUZAQdXW19hBQEVKUiwp
Jyri4+Tl4ygqLAoEt+3umQQLDwXv9ZoGEBYJ9vYJFxwK+NVT0AEEA4HvGIQ44QChOwcoXERw2C7C
CxkSKN6SMMPGBI2lKNzQUQGkJgs7hmAwmQkDkSQbWALYkOQJjROOHuncydNEiRM0nlAaSnRoIAA7
"



image create photo img_simulate -data "
R0lGODlhGAAYAPUAAAAAAAEBAQQEBAkJCQ8PDxoaGhsbGx4eHi8vLzY2Njw8PD09PUVFRUxMTE1N
TVVVVV5eXnZ2dnp6ep2dnaCgoKGhoaioqL6+vsHBwcLCwtDQ0NHR0dbW1tfX19ra2tzc3OPj4+Tk
5OXl5ejo6Onp6e/v7/Hx8fj4+Pn5+fr6+vv7+/7+/v///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAAAAAAALAAAAAAYABgAAAZ7QJZw
SCwaj8ikcslsKlMajslpHCkaFtKJOhQdBAbIBbXihgwAACEh8aio5zQA/JiAnHE5IICQZEpMeXoA
AwwVH29JgoMABRGAimiMaY6QSItyhRSISph8EhiWkXJ0dniSa22JTV5gYmRcIwsOWVtcLCkbHVO3
vb6/wFRBADs=
"

image create photo img_netlist -data "
R0lGODlhGAAYAPZcAAAAAAEBAQICAgQEBAUFBQcHBwgICAsLCwwMDA0NDQ4ODhERERoaGh4eHh8f
HyAgICEhISIiIiUlJSgoKCkpKSsrKy0tLS4uLi8vLzc3Nzw8PD4+Pj8/P0BAQEFBQUZGRkdHR0hI
SExMTFJSUlRUVFZWVlhYWFxcXGBgYGVlZWZmZmhoaHV1dXZ2dnd3d3l5eX5+fn9/f4KCgoaGhoeH
h4mJiZaWlpeXl5mZmaCgoKGhoaOjo6+vr7CwsLGxsbW1tba2trq6uru7u7y8vL6+vsDAwMHBwcLC
wsPDw8XFxc/Pz9nZ2dra2tvb29zc3N7e3uHh4ePj4+np6fPz8/f39/j4+Pn5+fr6+vv7+/z8/P39
/f7+/v///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
BAAAAAAALAAAAAAYABgAAAf+gFyCg4NaVodWWoSLjIM+Gh0dGj6NlYM2AJkANpaWmJqcnY2fmaGi
i6Sbp4yppquCra9UQi0rG5oAGyssQFSdVEEiCriZCCE/vp1aRSUJuAgjQoqrRyYGmQUkRJ1bWVdY
W1xZSScEAyVGWVxbWFdZ4YNLMx8pTOpVSCgoRr5ZTCkfaCwRtOXJCwgVbkiZNkWJkimCtEjBcSFC
iyZboMiYkCGHOlFadmSQEANKjQgHUAwxcgRKFUZVoBwxMgTFgQg1GlwTQICAgRZOGDlxYaCnAAAG
HDi4pilABR5crki9wqWHhQC4DDzwsIBYAQocNojdwIFCAWILPKhgQKyt20wiDFTowPC2riYMOqLA
AMEhkt+/gCNxAAEjCjtEiBMrPgQuEAA7
"

image create photo img_waves -data "
R0lGODlhGAAYAPcAAAAAAAEBAQICAgMDAwQEBAUFBQYGBgcHBwgICAkJCQoKCgsLCwwMDA0NDQ4O
Dg8PDxAQEBMTExQUFBUVFRYWFhcXFxoaGhwcHB0dHR8fHyEhISIiIiMjIyUlJScnJykpKSoqKi0t
LTAwMDExMTIyMjMzMzQ0ND09PT8/P0BAQEFBQUREREZGRkdHR0hISElJSUpKSktLS1RUVFVVVVZW
VldXV1hYWFpaWltbW1xcXF1dXV9fX2FhYWNjY2RkZGVlZWZmZmdnZ2lpaWtra2xsbG1tbW5ubm9v
b3BwcHFxcXJycnR0dHV1dXZ2dnd3d3t7e35+fn9/f4ODg4SEhIaGhoeHh4iIiImJiYqKiouLi4yM
jI2NjY6Ojo+Pj5OTk5aWlpiYmJmZmZqamp2dnaSkpKenp6ioqKmpqaysrLCwsLGxsbKysrS0tLi4
uLm5ubq6uru7u729vb6+vr+/v8HBwcTExMXFxcfHx8jIyMnJycrKyszMzM3Nzc7Ozs/Pz9DQ0NHR
0dLS0tPT09TU1NXV1dbW1tfX19jY2NnZ2dra2tvb29zc3N3d3d7e3uDg4OHh4eLi4uTk5OXl5ebm
5ufn5+jo6Onp6erq6uvr6+zs7O3t7e/v7/Hx8fLy8vPz8/T09Pf39/j4+Pn5+fr6+vv7+/z8/P39
/f7+/v///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAAAAAAALAAAAAAYABgA
AAj+AFEJHEiwoMGDCBMeLJVJUB9GnRQaJKWIzJMdN4RQQUNo1MFEcgadQvXpThIMAhQ0KBAggxA1
iDiZQnXK058mKqCQMgXHxYEHH2rwYNEBQoIIN8wcquQojQsHEICMGnXDAAYkhC5pwtQHiwkFCyJs
EPGBAoIIPwidKiWGiBhIMwWOsqSnC4wIAAAEQLAizCJSNCUhuoRqlKRHm0qhMmWpDpksUaqMoZPJ
4CQ1WZQUefLFjSPFqERxAjUS0p5FI0MFmhJCwACWCkxAmfMI1EBRiazU0FLKFB4aDBZgQAGjhAUH
CTQgiRMJ06Q6OCYs2DFVh4IJOexMwhRJjhINCRzjWCDR4sSFBQtm5DFVaouNK4pEDQQFSc6TrwEI
CBAwooqgUDQxwgckNH3SiUeokNJIG188YQQTXLBBiUGYyCGGFFFoQYYdmMxkyiaPVKKYJYdEMhIp
i3yRggF5ASBBDF7oYQmCi0ECxhBilHJKIUFIgEAFIIzAQQQLKFCCFXxgosklgxxxwQE4TOXDAw+8
sEaIiJyBgwQJQMDBCjvI4IEDBrDwhimkQKGCE4B4MtIpnSCiRhAZCEAAAwkIoMESeHxC0yBzKHLQ
KIOgUYUQOPQARRmLgCbRQJ4w4ochiT1q6aWWBgQAOw==
"
