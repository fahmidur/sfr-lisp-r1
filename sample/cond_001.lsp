#lang racket
(begin
 (define x 5)
 (print "x is " x)
 (if (> x 8)
  (print "x is > 8")
  (print "x is <= 8"))
)
