package main
 
import (
    "fmt"
    "math/rand"
)
 
func main() {
 
    /*
    Operadores básicos
    */
    a:= rand.Float32() * 100
    b:= rand.Float32() * 100
 
    fmt.Println("a = ", a)
    fmt.Println("b = ", b)
    fmt.Println("a+b = ", a+b)
    fmt.Println("a-b = ", a-b)
    fmt.Println("a*b = ", a*b)  
    fmt.Println("a/b = ", a/b)
    fmt.Println("a%b = ", a%b)
    fmt.Println("a>b = ", a>b)
    fmt.Println("a<b = ", a<b)
    fmt.Println("a>=b = ", a>=b)
    fmt.Println("a<=b = ", a<=b)
    fmt.Println("true&&true = ", true && false)
 
}