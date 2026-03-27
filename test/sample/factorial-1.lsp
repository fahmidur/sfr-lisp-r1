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
  (displayln (factorial 5 1))
)
