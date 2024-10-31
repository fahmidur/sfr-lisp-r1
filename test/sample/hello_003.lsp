#lang racket

(begin
  (define bla (lambda (x) (display x) (newline)))
  (bla "hello world")
)

