package elements

import (
	"math/rand"
	"time"

	"github.com/hajimehoshi/ebiten"
	"github.com/hajimehoshi/ebiten/ebitenutil"
)

// Enemy snake
type Enemy struct {
	game      *Game
	length    int
	direction string
	headLeft  ebiten.Image
	headUp    ebiten.Image
	headDown  ebiten.Image
	headRight ebiten.Image
	body      ebiten.Image
	parts     [][]float64
	newLength int
	points    int
	behaviour chan int
	seed      rand.Source
	alive     bool
}

// SpawnEnemy spawns a enemy snake in the board
func SpawnEnemy(game *Game) *Enemy {
	enemy := Enemy{
		game:      game,
		length:    0,
		direction: "up",
		newLength: 0,
		alive:     true,
	}

	enemy.behaviour = make(chan int)
	enemy.seed = rand.NewSource(time.Now().UnixNano())
	r := rand.New(enemy.seed)
	x := float64(r.Intn(54) * 20)     //1080
	y := float64(r.Intn(29)*20 + 120) //720

	headUp, _, _ := ebitenutil.NewImageFromFile("files/snake/enemySnakeUp.png", ebiten.FilterDefault)
	headDown, _, _ := ebitenutil.NewImageFromFile("files/snake/enemySnakeDown.png", ebiten.FilterDefault)
	headRight, _, _ := ebitenutil.NewImageFromFile("files/snake/enemySnakeRight.png", ebiten.FilterDefault)
	headLeft, _, _ := ebitenutil.NewImageFromFile("files/snake/enemySnakeLeft.png", ebiten.FilterDefault)
	body, _, _ := ebitenutil.NewImageFromFile("files/snake/enemySnakeBody.png", ebiten.FilterDefault)

	enemy.headUp = *headUp
	enemy.headDown = *headDown
	enemy.headRight = *headRight
	enemy.headLeft = *headLeft
	enemy.body = *body
	enemy.parts = append(enemy.parts, []float64{x, y})

	return &enemy
}

// Behaviour pipe
func (enemy *Enemy) Behaviour() error {
	for {
		dotTime := <-enemy.behaviour
		enemy.Update(dotTime)
	}
}

// Update enemy snake
func (enemy *Enemy) Update(dotTime int) error {
	if enemy.alive && enemy.game.alive {
		if dotTime == 1 {
			r := rand.New(enemy.seed)
			changeDirection := r.Intn(5)
			direction := r.Intn(3)
			x, y := enemy.getHead()
			if changeDirection == 3 {
				switch direction {
				case 0: //go up
					if y > 160 && enemy.direction != "down" {
						enemy.direction = "up"
					} else {
						enemy.direction = "down"
					}
				case 1: //go left
					if x > 40 && enemy.direction != "right" {
						enemy.direction = "left"
					} else {
						enemy.direction = "right"
					}

				case 2: //go right
					if x < 1040 && enemy.direction != "left" {
						enemy.direction = "right"
					} else {
						enemy.direction = "left"
					}
				case 3: //go up
					if y < 680 && enemy.direction != "up" {
						enemy.direction = "down"
					} else {
						enemy.direction = "up"
					}
				}
			}
			if y <= 160 {
				enemy.direction = "down"
			}
			if y >= 680 {
				enemy.direction = "up"
			}
			if x >= 1040 {
				enemy.direction = "left"
			}
			if x <= 40 {
				enemy.direction = "right"
			}
		}

		if dotTime == 1 {
			x, y := enemy.getHead()
			if enemy.enemyCollision(x, y) {
				enemy.alive = false

				for i := 0; i < len(enemy.parts); i++ {
					enemy.parts[i][0] = -20
					enemy.parts[i][1] = -20
				}
				enemy.game.enemyDied()
				if enemy.game.gui.remainingEnemies == 0 {
					enemy.game.GameOver()
				}
			}
			x2, y2 := enemy.game.player.getHead()
			if enemy.playerCollision(x2, y2) {
				enemy.game.GameOver()
				enemy.game.Crashed()
			}
		}
	}
	return nil
}

//Draw enemy
func (enemy *Enemy) Draw(screen *ebiten.Image, dotTime int) error {
	if enemy.game.alive {
		enemy.UpdatePos(dotTime)
	}
	drawer := &ebiten.DrawImageOptions{}
	x, y := enemy.getHead()
	drawer.GeoM.Translate(x, y)

	if enemy.direction == "up" {
		screen.DrawImage(&enemy.headUp, drawer)
	} else if enemy.direction == "down" {
		screen.DrawImage(&enemy.headDown, drawer)
	} else if enemy.direction == "right" {
		screen.DrawImage(&enemy.headRight, drawer)
	} else if enemy.direction == "left" {
		screen.DrawImage(&enemy.headLeft, drawer)
	}

	for i := 0; i < enemy.length; i++ {
		bodyDrawer := &ebiten.DrawImageOptions{}
		x, y := enemy.getPart(i)
		bodyDrawer.GeoM.Translate(x, y)
		screen.DrawImage(&enemy.body, bodyDrawer)
	}

	return nil
	/*
		finished := true
		if enemy.alive {
			finished = false
			if enemy.game.alive {
				enemy.UpdatePos(dotTime)
			}
			drawer := &ebiten.DrawImageOptions{}
			x, y := enemy.getHead()
			drawer.GeoM.Translate(x, y)

			if enemy.direction == "up" {
				screen.DrawImage(&enemy.headUp, drawer)
			} else if enemy.direction == "down" {
				screen.DrawImage(&enemy.headDown, drawer)
			} else if enemy.direction == "right" {
				screen.DrawImage(&enemy.headRight, drawer)
			} else if enemy.direction == "left" {
				screen.DrawImage(&enemy.headLeft, drawer)
			}

			for i := 0; i < enemy.length; i++ {
				bodyDrawer := &ebiten.DrawImageOptions{}
				x, y := enemy.getPart(i)
				bodyDrawer.GeoM.Translate(x, y)
				screen.DrawImage(&enemy.body, bodyDrawer)
			}
			return nil
		} else if !enemy.alive && !finished {
			for i := 0; i < len(enemy.parts); i++ {
				enemy.parts[i][0] = -20
				enemy.parts[i][1] = -20
			}
			fmt.Printf("stuck \n")
			finished = true
		}
	*/

}

// UpdatePos moves enemy snake
func (enemy *Enemy) UpdatePos(dotTime int) {
	if dotTime == 1 {
		if enemy.newLength > 0 {
			enemy.length++
			enemy.newLength--
		}
		switch enemy.direction {
		case "up":
			enemy.turn(0, -20)
		case "down":
			enemy.turn(0, 20)
		case "right":
			enemy.turn(20, 0)
		case "left":
			enemy.turn(-20, 0)
		}
	}
}

func (enemy *Enemy) ateFood() {
	enemy.points++
	enemy.newLength++
}

func (enemy *Enemy) getHead() (float64, float64) {
	return enemy.parts[0][0], enemy.parts[0][1]
}

func (enemy *Enemy) getPart(pos int) (float64, float64) {
	return enemy.parts[pos+1][0], enemy.parts[pos+1][1]
}

func (enemy *Enemy) turn(x, y float64) {
	x2 := enemy.parts[0][0] + x
	y2 := enemy.parts[0][1] + y
	enemy.updateEnemy(x2, y2)
}

func (enemy *Enemy) updateEnemy(x, y float64) {
	enemy.parts = append([][]float64{{x, y}}, enemy.parts...)
	enemy.parts = enemy.parts[:enemy.length+1]
}

func (enemy *Enemy) playerCollision(x, y float64) bool {
	for i := 0; i < len(enemy.parts); i++ {
		if x == enemy.parts[i][0] && y == enemy.parts[i][1] {
			return true
		}
	}
	return false
}

func (enemy *Enemy) enemyCollision(x, y float64) bool {
	for i := 0; i < len(enemy.game.player.parts); i++ {
		if x == enemy.game.player.parts[i][0] && y == enemy.game.player.parts[i][1] {
			return true
		}
	}
	return false
}
