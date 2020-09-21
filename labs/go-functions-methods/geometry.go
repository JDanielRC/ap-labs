// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

// Point : style
type Point struct{ x, y float64 }

// Distance : traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

// Distance : same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

// X returns x value of point
func (p Point) X() float64 {
	return p.x
}

// Y returns y value of point
func (p Point) Y() float64 {
	return p.y
}

// A Path is a journey connecting the points with straight lines.
type Path []Point

func generatePoint() Point {
	seed := rand.NewSource(time.Now().UnixNano())
	random := rand.New(seed)
	return Point{math.Round((((random.Float64() * 200) - 100) * 100) / 100), math.Round((((random.Float64() * 200) - 100) * 100) / 100)}
}

func orientation(p, q, r Point) int {
	val := (q.y-p.y)*(r.x-q.x) - (q.x-p.x)*(r.y-q.y)

	if val == 0 {
		return 0
	}

	if val > 0 {
		return 1
	}

	return 2
}

func onSegment(p, q, r Point) bool {
	if q.x <= math.Max(p.x, r.x) && q.x >= math.Min(p.x, r.x) &&
		q.y <= math.Max(p.y, r.y) && q.y >= math.Min(p.y, r.y) {
		return true
	}

	return false
}

func doIntersect(p1, q1, p2, q2 Point) bool {
	// orientation for each point
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)

	// General case
	if o1 != o2 && o3 != o4 {
		return true
	}

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if o1 == 0 && onSegment(p1, p2, q1) {
		return true
	}

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if o2 == 0 && onSegment(p1, q2, q1) {
		return true
	}

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if o3 == 0 && onSegment(p2, p1, q2) {
		return true
	}

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if o4 == 0 && onSegment(p2, q1, q2) {
		return true
	}

	return false
}

func figure(nvertex int) Path {
	auxPath := Path{}
	flag := true
	var point Point
	for i := 0; i < nvertex; i++ {
		//first 2 points won't intersect
		if i <= 2 {
			point = generatePoint()
			auxPath = append(auxPath, point)
		} else {
			for flag {
				point = generatePoint()
				for j := 0; j < len(auxPath)-2; j++ {
					intersect := doIntersect(auxPath[j], auxPath[j+1], auxPath[i-1], point)
					if !intersect {
						flag = false
					}
				}
			}
			auxPath = append(auxPath, point)
			flag = true
		}

	}
	return auxPath
}

//!-point

//!+path

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

func main() {
	if len(os.Args) > 1 {
		nvertex, _ := strconv.Atoi(os.Args[1])
		if nvertex < 3 {
			fmt.Println("Can't make a figure with less than 3 sides.")
			return
		}
		fmt.Printf("- Generating a [%d] sides figure \n", nvertex)
		fmt.Println("- Figure's vertices")
		path := figure(nvertex)
		for i := 0; i < len(path); i++ {
			fmt.Printf("  - (%.0f, %.0f) \n", path[i].X(), path[i].Y())
		}
		var perimeter float64 = 0
		var distance float64 = 0
		fmt.Println("Figure's Perimeter")
		fmt.Printf("  - ")
		for i := 0; i < len(path); i++ {
			if i == len(path)-1 {
				distance = Distance(path[i], path[0])
				perimeter += distance
				fmt.Printf("%.2f = ", distance)

			} else {
				distance = Distance(path[i], path[i+1])
				perimeter += distance
				fmt.Printf("%.2f + ", distance)
			}
		}
		fmt.Printf("%.2f \n", perimeter)
	} else {
		fmt.Println("Incorrect number of arguments, input only number of sides in figure (>=3).")
	}

}

//!-path
