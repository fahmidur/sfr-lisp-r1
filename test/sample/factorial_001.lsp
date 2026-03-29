#lang racket
(begin
  (define factorial (lambda (x acc) 
    (if (<= x 1)
      acc
      (
        factorial
        (- x 1) (* acc x) 
      )
    )
  ))
  (displayln (factorial 1 1))
  (displayln (factorial 2 1))
  (displayln (factorial 3 1))
  (displayln (factorial 4 1))
  (displayln (factorial 5 1))
)
