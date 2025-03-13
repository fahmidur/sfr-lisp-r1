#lang racket
(begin
  (displayln "--- (define make_account ...) ---")
  (define make_account (lambda (balance) (lambda (amt) (begin (set! balance (+ balance amt)) balance ))))
  (displayln "--- (define acc (make_account 100) ---")
  (define acc (make_account 100))
  (displayln "--- (acc -20) ---")
  (displayln (acc -20))
)
