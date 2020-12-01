package main

import (
	"elements/elements"
	"fmt"
	"log"
	"os"
	"strconv"

	"github.com/hajimehoshi/ebiten"
)

var game elements.Game

func init() {
	if len(os.Args) != 3 {
		fmt.Printf("Incorrect arguments, first argument takes number of food and second one number of enemies \n")
		os.Exit(1)
	}
	foods, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Printf("Incorret argument %s for number of foods, try with a number. \n", os.Args[1])
		os.Exit(1)
	}
	enemies, err2 := strconv.Atoi(os.Args[2])
	if err2 != nil {
		fmt.Printf("Incorret argument %s for number of enemies, try with a number. \n", os.Args[2])
		os.Exit(1)
	}
	fmt.Print(enemies)
	game = elements.Start(foods, enemies)
}

// Game implements ebiten.Game interface.
type Game struct{}

// Update proceeds the game state.
// Update is called every tick (1/60 [s] by default).
func (g *Game) Update(screen *ebiten.Image) error {
	if err := game.Update(); err != nil {
		return err
	}
	return nil
}

// Draw draws the game screen.
// Draw is called every frame (typically 1/60[s] for 60Hz display).
func (g *Game) Draw(screen *ebiten.Image) {
	if err := game.Draw(screen); err != nil {
		fmt.Println(err)
	}
}

// Layout takes the outside size (e.g., the window size) and returns the (logical) screen size.
// If you don't have to adjust the screen size with the outside size, just return a fixed size.
func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 1080, 720
}

func main() {
	game := &Game{}

	ebiten.SetWindowSize(1080, 720)
	ebiten.SetWindowTitle("Snake")

	if err := ebiten.RunGame(game); err != nil {
		log.Fatal(err)
	}

}
