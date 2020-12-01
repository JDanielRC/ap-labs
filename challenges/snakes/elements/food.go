package elements

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/hajimehoshi/ebiten/ebitenutil"

	"github.com/hajimehoshi/ebiten"
)

// Food stores the game Object, whatever image it might take, plus the limit coordinates where it can appear plus the position it will be spawned at
type Food struct {
	game   *Game
	image  ebiten.Image
	xLimit int
	yLimit int
	foodX  float64
	foodY  float64
	active bool
}

// RandomFood chooses a random image from files, and then sets a position for it on the screen
func RandomFood(game *Game) *Food {
	food := Food{
		game:   game,
		xLimit: 53,
		yLimit: 30,
		active: true,
	}
	s1 := rand.NewSource(time.Now().UnixNano())
	r1 := rand.New(s1)
	rand := r1.Intn(4)
	fmt.Printf("number: %d \n", rand)
	switch rand {
	case 0:
		itemFood, _, _ := ebitenutil.NewImageFromFile("files/hamburger2.png", ebiten.FilterDefault)
		food.image = *itemFood
	case 1:
		itemFood, _, _ := ebitenutil.NewImageFromFile("files/cupcake2.png", ebiten.FilterDefault)
		food.image = *itemFood
	case 2:
		itemFood, _, _ := ebitenutil.NewImageFromFile("files/eggnog2.png", ebiten.FilterDefault)
		food.image = *itemFood
	case 3:
		itemFood, _, _ := ebitenutil.NewImageFromFile("files/syrup2.png", ebiten.FilterDefault)
		food.image = *itemFood
	default:
		itemFood, _, _ := ebitenutil.NewImageFromFile("files/hamburger2.png", ebiten.FilterDefault)
		food.image = *itemFood
	}
	food.foodX = float64(r1.Intn(food.xLimit)*20 + 20)
	food.foodY = float64(r1.Intn(food.yLimit)*20 + 120)
	fmt.Printf("valor en x: %f valor en y: %f \n", food.foodX, food.foodY)
	return &food
}

// Update for mechanics needed in ebit library
func (food *Food) Update(dotTime int) error {
	if !food.active {
		return nil
	}
	return nil
}

// Draw from ebiten library, draws food items
func (food *Food) Draw(screen *ebiten.Image, dotTime int) error {
	drawer := &ebiten.DrawImageOptions{}
	drawer.GeoM.Translate(food.foodX, food.foodY)
	screen.DrawImage(&food.image, drawer)
	return nil
}
