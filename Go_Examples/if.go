package main
 
import (
    "fmt"
)
 
func main() {
 
    /*
    Prueba con if
    */
    var a, b int = 5, 8
    
    if a%2 == 0 {
        fmt.Println("a is even")
    } else {
        fmt.Println("a is odd")
    }
    
    if b%4 == 0 {
        fmt.Println("8 is divisible by 4")
    }
 
    if num := 9; num < 0 {
        fmt.Println(num, "is negative")
    } else if num < 10 {
        fmt.Println(num, "has 1 digit")
    } else {
        fmt.Println(num, "has multiple digits")
    }
}