#lang racket
(begin
  (define fib (lambda (x) 
    (if (<= x 1)
      (quote 1)
      (+ 
        (fib (- x 1))
        (fib (- x 2))
      )
    )
  ))
  (displayln (fib 3))
)
