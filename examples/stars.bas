INPUT "What is your name: "; U$
PRINT "Hello "; U$

Start:
INPUT "How many stars do you want: "; N
S$ = ""
FOR I = 1 TO N
  S$ = S$ + "*"
NEXT I
PRINT S$

Again:
INPUT "Do you want more stars? "; A$
IF LEN(A$) = 0 THEN GOTO Again
A$ = LEFT$(A$, 1)
IF A$ = "Y" OR A$ = "y" THEN GOTO Start
PRINT "Goodbye "; U$
END