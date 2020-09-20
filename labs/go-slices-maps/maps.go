package main

import (
	"strings"

	"golang.org/x/tour/wc"
)

func WordCount(s string) map[string]int {
	words := strings.Fields(s)
	mapping := make(map[string]int)
	for i := range words {
		mapping[words[i]]++
	}
	return mapping
}

func main() {
	wc.Test(WordCount)
}
