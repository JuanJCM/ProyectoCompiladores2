package main
 
import "fmt"
 
func BubbleSort(array[] int, size int)[]int {
   for i:=0; i< size-1; i++ {
      for j:=0; j < size-i-1; j++ {
         if (array[j] > array[j+1]) {
            array[j] = array[j+1]
            array[j+1] = array[j]
         }
      }
   }
   return array
}

func main() {
 
   var size int = 7 
   array:= []int{11, 14, 3, 8, 18, 17, 43};
     fmt.Println(BubbleSort(array, size))
}