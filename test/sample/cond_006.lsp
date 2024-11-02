#lang racket
(begin
 (define x 5)
 (display "x is ")
 (displayln x)
 (define y 6)
 (display "y is ")
 (displayln y)
 (if (equal? x y)
  (displayln "x is equal to y")
  (displayln "x is not equal to y"))
)
