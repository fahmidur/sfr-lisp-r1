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
  (displayln (fib 1))
  (displayln (fib 2))
  (displayln (fib 3))
  (displayln (fib 4))
  (displayln (fib 5))
  (displayln (fib 6))
  (displayln (fib 7))
)
