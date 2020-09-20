package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	array := make([][]uint8, dy)
	for i := 0; i < dy; i++ {
		array[i] = make([]uint8, dx)
		for j := 0; j < dx; j++ {
			array[i][j] = uint8(i ^ j + ((i + j) / 2) + (i * j))
		}
	}
	return array
}

func main() {
	pic.Show(Pic)
}
