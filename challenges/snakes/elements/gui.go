package elements

import (
	"image/color"
	"strconv"

	"github.com/hajimehoshi/ebiten"
	"github.com/hajimehoshi/ebiten/text"
	"golang.org/x/image/font/inconsolata"
)

// GUI contains all info for the screen
type GUI struct {
	score            int
	remainingEnemies int
	game             *Game
	largest          bool
}

func initializeGUI(g *Game) *GUI {
	gui := GUI{
		score:            0,
		remainingEnemies: g.enemiesAmount,
		game:             g,
		largest:          false,
	}
	return &gui
}

func (gui *GUI) ateFood() {
	gui.score++
}

func (gui *GUI) enemyDied() {
	gui.remainingEnemies--
}

// Draw GUI
func (gui *GUI) Draw(screen *ebiten.Image) error {
	text.Draw(screen, "Current Score: "+strconv.Itoa(gui.score), inconsolata.Bold8x16, 90, 20, color.Black)
	text.Draw(screen, "Remaining enemies: "+strconv.Itoa(gui.remainingEnemies), inconsolata.Bold8x16, 780, 20, color.Black)
	text.Draw(screen, "Art by Juanpy", inconsolata.Bold8x16, 10, 715, color.Black)
	if gui.game.alive == false {
		textGameOver := ""
		if gui.game.crashed {
			textGameOver = "          Game over\nYou lose. You crashed against something :("
		} else if gui.remainingEnemies <= 0 {
			textGameOver = "          Game over\nYou win. You killed all the other snakes :D"
		} else if gui.largest {
			textGameOver = "          Game over\nYou win. You ate the most food :D"
		} else {
			textGameOver = "          Game over\nYou lose. You did not ate the most food :("
		}
		text.Draw(screen, textGameOver, inconsolata.Bold8x16, 400, 300, color.Black)
	}
	return nil
}
