#lang racket
(begin
  (define foo
    (lambda (x)
      (display "foo: ")
      (display x)
      (newline)))
  (foo "hello world")
)

