package elements

import (
	"github.com/hajimehoshi/ebiten"
	"github.com/hajimehoshi/ebiten/ebitenutil"
)

// Player is the snake controlled by the user
type Player struct {
	game      *Game
	length    int
	newLength int
	points    int
	direction string
	headLeft  ebiten.Image
	headUp    ebiten.Image
	headDown  ebiten.Image
	headRight ebiten.Image
	body      ebiten.Image
	parts     [][]float64
	behaviour chan int
}

// Spawn creates the snake for the player to move
func Spawn(game *Game) *Player {
	player := Player{
		game:      game,
		length:    0,
		direction: "up",
		newLength: 0,
	}
	player.behaviour = make(chan int)
	player.parts = append(player.parts, []float64{300, 300})
	headUp, _, _ := ebitenutil.NewImageFromFile("files/snake/snakeUp.png", ebiten.FilterDefault)
	headDown, _, _ := ebitenutil.NewImageFromFile("files/snake/snakeDown.png", ebiten.FilterDefault)
	headRight, _, _ := ebitenutil.NewImageFromFile("files/snake/snakeRight.png", ebiten.FilterDefault)
	headLeft, _, _ := ebitenutil.NewImageFromFile("files/snake/snakeLeft.png", ebiten.FilterDefault)
	body, _, _ := ebitenutil.NewImageFromFile("files/snake/snakeBody.png", ebiten.FilterDefault)
	player.headUp = *headUp
	player.headDown = *headDown
	player.headRight = *headRight
	player.headLeft = *headLeft
	player.body = *body
	return &player
}

// Behaviour pipe
func (player *Player) Behaviour() error {
	dotTime := <-player.behaviour
	for {
		player.Update(dotTime)
		dotTime = <-player.behaviour
	}
}

//Update will detect player movement accordingly
func (player *Player) Update(dotTime int) error {
	if player.game.alive {
		if ebiten.IsKeyPressed(ebiten.KeyRight) && player.direction != "right" && player.direction != "left" {
			player.direction = "right"
			return nil
		} else if ebiten.IsKeyPressed(ebiten.KeyLeft) && player.direction != "left" && player.direction != "right" {
			player.direction = "left"
			return nil
		} else if ebiten.IsKeyPressed(ebiten.KeyDown) && player.direction != "down" && player.direction != "up" {
			player.direction = "down"
			return nil
		} else if ebiten.IsKeyPressed(ebiten.KeyUp) && player.direction != "up" && player.direction != "down" {
			player.direction = "up"
			return nil
		}

		if dotTime == 1 {
			x, y := player.getHead()
			if x < 0 || x > 1060 || y < 120 || y > 700 || player.ownCollision() {
				player.game.GameOver()
				player.game.Crashed()
			}
		}
	}
	return nil

}

// Draw the player's snake, according to their direction
func (player *Player) Draw(screen *ebiten.Image, dotTime int) error {
	if player.game.alive {
		player.UpdatePos(dotTime)
	}

	drawer := &ebiten.DrawImageOptions{}
	x, y := player.getHead()
	drawer.GeoM.Translate(x, y)

	if player.direction == "up" {
		screen.DrawImage(&player.headUp, drawer)
	} else if player.direction == "down" {
		screen.DrawImage(&player.headDown, drawer)
	} else if player.direction == "right" {
		screen.DrawImage(&player.headRight, drawer)
	} else if player.direction == "left" {
		screen.DrawImage(&player.headLeft, drawer)
	}

	for i := 0; i < player.length; i++ {
		partDrawer := &ebiten.DrawImageOptions{}
		x, y := player.getPart(i)
		partDrawer.GeoM.Translate(x, y)
		screen.DrawImage(&player.body, partDrawer)
	}

	return nil
}

// UpdatePos will allow the player to make a turn towards a certain direction
func (player *Player) UpdatePos(dotTime int) {
	if dotTime == 1 {
		if player.newLength > 0 {
			player.length++
			player.newLength--
		}
		switch player.direction {
		case "left":
			player.turn(-20, 0)
		case "right":
			player.turn(20, 0)
		case "up":
			player.turn(0, -20)
		case "down":
			player.turn(0, 20)
		}
	}
}

func (player *Player) getHead() (float64, float64) {
	return player.parts[0][0], player.parts[0][1]
}

func (player *Player) ateFood() {
	player.points++
	player.newLength++
}

func (player *Player) getPart(pos int) (float64, float64) {
	return player.parts[pos+1][0], player.parts[pos+1][1]
}

func (player *Player) turn(x, y float64) {
	x2 := player.parts[0][0] + x
	y2 := player.parts[0][1] + y
	player.updateSnake(x2, y2)
}

func (player *Player) updateSnake(x, y float64) {
	player.parts = append([][]float64{{x, y}}, player.parts...)
	player.parts = player.parts[:player.length+1]
}

func (player *Player) ownCollision() bool {
	x, y := player.getHead()
	for i := 1; i < len(player.parts); i++ {
		if x == player.parts[i][0] && y == player.parts[i][1] {
			return true
		}
	}
	return false
}
