#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iostream>

enum GameState {
    Intro, Home, LevelSelect, Instructions, Help, About, Settings, Playing, Paused, GameOver, Blast
};
class Missile {
public:
    sf::Sprite sprite;
    float speed;

    Missile(const sf::Texture& texture, float x, float y) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.1f, 0.1f); // Scale down the missile to fit
        speed = 500.0f; // Adjust speed as needed
    }

    void update(float deltaTime) {
        sprite.move(speed * deltaTime, 0); // Move to the right
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

class EnemyHelicopter {
public:
    sf::Sprite sprite;
    std::vector<sf::Texture> textures;
    float animationTimer;
    int currentFrame;

    EnemyHelicopter(std::vector<sf::Texture> textures, float startX, float startY): textures(textures), animationTimer(0), currentFrame(0) {
        sprite.setTexture(textures[0]);
        sprite.setScale(0.3f, 0.3f); // Adjust size as needed
        sprite.setPosition(startX, startY);
    }

    void update(float deltaTime) {
        // Move left
        sprite.move(-250 * deltaTime, 0);

        // Animate the enemy helicopter
        animationTimer += deltaTime;
        if (animationTimer >= 0.03f) { // Change frame every 0.1 seconds
            animationTimer = 0;
            currentFrame = (currentFrame + 1) % textures.size();
            sprite.setTexture(textures[currentFrame]);
        }
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};
class Coin {
public:
    sf::Sprite sprite;

    Coin(const sf::Texture& texture, float startX, float startY) {
        sprite.setTexture(texture);
        sprite.setScale(0.15f, 0.15f); // Adjust size as needed
        sprite.setPosition(startX, startY);
    }

    void update(float deltaTime) {
        sprite.move(-200 * deltaTime, 0); // Move left
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};


class ScrollingBackground {
public:
    ScrollingBackground(float speed) : speed(speed) {
        if (!texture.loadFromFile("assets/Asset1.png")) {
            // Handle loading error
        }

        float textureWidth = texture.getSize().x;

        // Create two sprites to cover the screen
        for (int i = 0; i < 2; ++i) {
            sf::Sprite sprite;
            sprite.setTexture(texture);
            sprite.setPosition(i * textureWidth, 0);
            sprites.push_back(sprite);
        }
    }

    void update(float deltaTime) {
        float textureWidth = texture.getSize().x;

        for (auto& sprite : sprites) {
            sprite.move(-speed * deltaTime, 0);

            // If the sprite moves off the screen, reset its position
            if (sprite.getPosition().x + textureWidth < 0) {
                sprite.setPosition(sprite.getPosition().x + 2 * textureWidth, 0);
            }
        }
    }

    void render(sf::RenderWindow& window) {
        for (const auto& sprite : sprites) {
            window.draw(sprite);
        }
    }

private:
    sf::Texture texture;
    std::vector<sf::Sprite> sprites;
    float speed;
};

class Helicopter {
public:
    std::vector<sf::Texture> textures;
    sf::Sprite sprite;
    float velocity;
    float animationTimer;
    int currentFrame;

    Helicopter(const std::vector<std::string>& textureFiles) {
        for (const auto& file : textureFiles) {
            sf::Texture texture;
            if (!texture.loadFromFile(file)) {
                // Handle loading error
            }
            textures.push_back(texture);
        }
        sprite.setTexture(textures[0]);

        float scaleX = 0.40f; // Adjust this value as needed
        float scaleY = 0.40f; // Adjust this value as needed
        sprite.setScale(scaleX, scaleY);

        sprite.setPosition(100, 300); // Start position
        velocity = 0;
        animationTimer = 0;
        currentFrame = 0;
    }

    void update(float deltaTime) {
        velocity += 800 * deltaTime; // Gravity
        sprite.move(0, velocity * deltaTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            velocity = -250; // Flap
        }

        animationTimer += deltaTime;
        if (animationTimer >= 0.03f) { // Change frame every 0.03 seconds
            animationTimer = 0;
            currentFrame = (currentFrame + 1) % textures.size();
            sprite.setTexture(textures[currentFrame]);
        }
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

class Obstacle {
public:
    sf::Sprite topPipe;
    sf::Sprite bottomPipe;
    static sf::Texture topPipeTexture;
    static sf::Texture bottomPipeTexture;

    Obstacle(float x, float gapY) {
        if (topPipeTexture.getSize().x == 0) {
            if (!topPipeTexture.loadFromFile("assets/Toppipe.png")) {
                // Handle loading error
            }
        }
        if (bottomPipeTexture.getSize().x == 0) {
            if (!bottomPipeTexture.loadFromFile("assets/Bottompipe.png")) {
                // Handle loading error
            }
        }

        topPipe.setTexture(topPipeTexture);
        bottomPipe.setTexture(bottomPipeTexture);

        // Desired pipe dimensions
        float desiredWidth = 75;
        float desiredHeight = 390;

        // Calculate scale factors
        float scaleX = desiredWidth / topPipeTexture.getSize().x;
        float scaleY = desiredHeight / topPipeTexture.getSize().y;

        // Apply scaling
        topPipe.setScale(scaleX, scaleY);
        bottomPipe.setScale(scaleX, scaleY);

        float gapSize = 250; // Adjust this value as needed

        topPipe.setPosition(x, gapY - gapSize / 2 - topPipe.getGlobalBounds().height);
        bottomPipe.setPosition(x, gapY + gapSize / 2);
    }

    void update(float deltaTime) {
        topPipe.move(-200 * deltaTime, 0);
        bottomPipe.move(-200 * deltaTime, 0);
    }

    sf::FloatRect getTopBounds() const {
        return topPipe.getGlobalBounds();
    }

    sf::FloatRect getBottomBounds() const {
        return bottomPipe.getGlobalBounds();
    }
};

// Define the static textures
sf::Texture Obstacle::topPipeTexture;
sf::Texture Obstacle::bottomPipeTexture;


int main() {
    sf::RenderWindow window(sf::VideoMode(1080, 640), "Helicopter Game");
    window.setFramerateLimit(60);

    std::vector<std::string> helicopterTextureFiles = {
        "assets/helicopter1.png",
        "assets/helicopter2.png",
        "assets/helicopter3.png",
        "assets/helicopter4.png"
    };
    Helicopter helicopter(helicopterTextureFiles);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        // Handle loading error
    }
    //Coin feature
	sf::Texture coinTexture;
	if (!coinTexture.loadFromFile("assets/coin.png")) {
    // Handle loading error
	}
    // Load music
    sf::Music music;
    if (!music.openFromFile("sfx/Helicopter flying.wav")) {
        // Handle loading error
    }
     // Load background music
    sf::Music Background;
    if (!Background.openFromFile("sfx/helicopter backgroung.wav")) {
        // Handle loading error
    }
    // Load coin collecting music
    // Load coin collection sound
	sf::SoundBuffer coinSoundBuffer;
	if (!coinSoundBuffer.loadFromFile("sfx/coin sound.wav")) {
    // Handle loading error
	}
	sf::Sound coinSound;
	coinSound.setBuffer(coinSoundBuffer);

    // Load intro image
    sf::Texture introTexture;
    if (!introTexture.loadFromFile("assets/Intro2.png")) {
        // Handle loading error
    }
    
    // Load intro image
    sf::Texture levelTexture;
    if (!levelTexture.loadFromFile("assets/level.png")) {
        // Handle loading error
    }
    // Load home image
    sf::Texture homeTexture;
    if (!homeTexture.loadFromFile("assets/home.png")) {
        // Handle loading error
    }
    // Load loading images
	std::vector<sf::Texture> loadingTextures(4);
	std::vector<sf::Sprite> loadingSprites(4);
	for (int i = 0; i < 4; ++i) {
    if (!loadingTextures[i].loadFromFile("assets/loading" + std::to_string(i + 1) + ".png")) {
        // Handle loading error
    }
    loadingSprites[i].setTexture(loadingTextures[i]);
	}
    // Load blast textures
	sf::Texture blastTexture1, blastTexture2;
	if (!blastTexture1.loadFromFile("assets/blast1.png") || !blastTexture2.loadFromFile("assets/blast2.png")) {	
	// Handle loading error
	}
	sf::Sprite blastSprite1(blastTexture1);
	sf::Sprite blastSprite2(blastTexture2);
	blastSprite1.setScale(helicopter.sprite.getScale()); // Match helicopter scale
	blastSprite2.setScale(helicopter.sprite.getScale()); // Match helicopter scale
	// Load blast sound
	sf::SoundBuffer blastBuffer;
	if (!blastBuffer.loadFromFile("sfx/blast.wav")) {
    // Handle loading error
	}
	sf::Sound blastSound;
	blastSound.setBuffer(blastBuffer);
	
	// Load enemy helicopter textures
    std::vector<sf::Texture> enemyTextures;
    for (int i = 1; i <= 4; ++i) {
        sf::Texture texture;
        if (!texture.loadFromFile("assets/en" + std::to_string(i) + ".png")) {
            // Handle error
        }
        enemyTextures.push_back(texture);
    }
	sf::Texture missileTexture;
	if (!missileTexture.loadFromFile("assets/missile.png")) {
    // Handle loading error
	}
	sf::SoundBuffer missileSoundBuffer;
	if (!missileSoundBuffer.loadFromFile("sfx/laser.wav")) {
    // Handle loading error
	}
	sf::Sound missileSound;
	missileSound.setBuffer(missileSoundBuffer);

	
    // Initialize game-related variables
    std::vector<Obstacle> obstacles;
    std::vector<Coin> coins;
	std::vector<EnemyHelicopter> enemyHelicopters;
	std::vector<Missile> missiles;

    float obstacleSpawnTimer = 0;
    const float obstacleSpawnInterval = 1.8f;
    float introTime = 0;
    float elapsedTime = 0; // Variable to keep track of the elapsed time
    int highScore = 0; // Variable to keep track of the high score
    float coinSpawnTimer = 0;
	const float coinSpawnInterval = 1.5f; // Adjust interval as needed
	float fadeSpeed = 100.0f; // Adjust fade speed
	float alpha = 0; // Start at 0 (fully transparent)
    int currentLoadingFrame = 0;
	float loadingAnimationTimer = 0; 
	int currentLevel=1;// 1 = Easy, 2 = Medium, 3 = Hard
    float blastTimer=0;
    bool isPaused = false;  // Track pause state
    bool fadeInComplete = false; // Track whether the fade-in has finished
    bool soundEnabled = true;
    bool musicEnabled = true;
    
    float enemySpawnTimer = 0;
    const float enemySpawnInterval = 3.0f;

    sf::Clock clock;
    srand(static_cast<unsigned>(time(0)));

    GameState gameState = Intro;

	//level button feature
	sf::Text easyButton, mediumButton, hardButton;
	// Easy Button
	easyButton.setFont(font);
	easyButton.setCharacterSize(32);
	easyButton.setFillColor(sf::Color::Green);
	easyButton.setString("Easy");
	easyButton.setPosition(500, 145);
	easyButton.setStyle(sf::Text::Bold);
	// Medium Button
	mediumButton.setFont(font);
	mediumButton.setCharacterSize(32);
	mediumButton.setFillColor(sf::Color::Yellow);
	mediumButton.setString("Medium");
	mediumButton.setPosition(486, 300);
	mediumButton.setStyle(sf::Text::Bold);
	// Hard Button
	hardButton.setFont(font);
	hardButton.setCharacterSize(32);
	hardButton.setFillColor(sf::Color::Red);
	hardButton.setString("Hard");
	hardButton.setPosition(500, 450);
	hardButton.setStyle(sf::Text::Bold);
    // Create text object for "Loading..." display
    sf::Text loadingText;
    loadingText.setFont(font);
    loadingText.setCharacterSize(18);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setString("Loading...");
    loadingText.setPosition(540, 610); // Adjust the position as needed

    // Create text object for score display
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(10, 10);
scoreText.setStyle(sf::Text::Bold);
    // Create text objects for the popup window
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(440, 200);

    sf::Text currentScoreText;
    currentScoreText.setFont(font);
    currentScoreText.setCharacterSize(24);
    currentScoreText.setFillColor(sf::Color::White);
    currentScoreText.setPosition(465, 250);

    sf::Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(sf::Color::White);
    highScoreText.setPosition(465, 280);

    sf::Text playAgainText;
    playAgainText.setFont(font);
    playAgainText.setCharacterSize(24);
    playAgainText.setFillColor(sf::Color::Green);
    playAgainText.setString("Play Again");
    playAgainText.setPosition(475, 350);

    sf::Text backButton;
    backButton.setFont(font);
    backButton.setCharacterSize(24);
    backButton.setFillColor(sf::Color::Cyan);
    backButton.setString("Main Menu");
    backButton.setPosition(10, 550);
	backButton.setStyle(sf::Text::Bold);
    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(24);
    helpText.setFillColor(sf::Color::White);
    helpText.setString("For any issue or suggestions email to:\nsizan.vvv@gmail.com");
    helpText.setPosition(300, 300);

    sf::Text instructionsText;
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setString("Instructions:\n1. Press Space to flap.\n2. Avoid the obstacles.\n3. Press 'P' from keybord or Pause button to pause the game!\n4. Press 'C' from keybord or Continue button to continue the game!\n5. Press the Right arrow Key for Shooting in the hard level\n6. Have a great time with the game!!!");
    instructionsText.setPosition(250, 200);

    sf::Text aboutText;
    aboutText.setFont(font);
    aboutText.setCharacterSize(24);
    aboutText.setFillColor(sf::Color::White);
    aboutText.setString("Programming Language: C++ \nLibrary: Simple and Fast Multimedia Library \nIDE: Visual Studio or Code Blocks or any C++ IDE with SFML support Target Platform \nOperating System: Windows \nCompatibility: Ensure the game runs smoothly on Windows-based systems.");
    aboutText.setPosition(100, 300);
    music.setLoop(true); // Loop the music
   music.setVolume(100); // Set volume to 100 (0 to 100)
   Background.setLoop(true); // Loop the music
   Background.setVolume(100); // Set volume to 100 (0 to 100)

    
    sf::Sprite introSprite(introTexture);
    sf::Sprite homeSprite(homeTexture);
    sf::Sprite levelSprite(levelTexture);
    // Create home buttons
    sf::Text playButton, helpButton, aboutButton, exitButton, instructionsButton, settingsButton;
    playButton.setFont(font);
    playButton.setCharacterSize(32);
    playButton.setFillColor(sf::Color::Yellow);
    playButton.setString("Play Game");
    playButton.setPosition(131, 452);
    playButton.setStyle(sf::Text::Bold);
	
    helpButton.setFont(font);
    helpButton.setCharacterSize(32);
    helpButton.setFillColor(sf::Color::Yellow);
    helpButton.setString("Help Us");
    helpButton.setPosition(160, 548);
    helpButton.setStyle(sf::Text::Bold);

    aboutButton.setFont(font);
    aboutButton.setCharacterSize(32);
    aboutButton.setFillColor(sf::Color::Yellow);
    aboutButton.setString("About");
    aboutButton.setPosition(500, 544);
    aboutButton.setStyle(sf::Text::Bold);

    instructionsButton.setFont(font);
    instructionsButton.setCharacterSize(32);
    instructionsButton.setFillColor(sf::Color::Yellow);
    instructionsButton.setString("Instructions");
    instructionsButton.setPosition(796, 453);
    instructionsButton.setStyle(sf::Text::Bold);
	
	settingsButton.setFont(font);
    settingsButton.setCharacterSize(32);
    settingsButton.setFillColor(sf::Color::Yellow);
    settingsButton.setString("Settings");
    settingsButton.setPosition(480, 453);
    settingsButton.setStyle(sf::Text::Bold);
	
    exitButton.setFont(font);
    exitButton.setCharacterSize(32);
    exitButton.setFillColor(sf::Color::Yellow);
    exitButton.setString("Exit");
    exitButton.setPosition(859, 544);
    exitButton.setStyle(sf::Text::Bold);
    
    // Create Pause and Continue buttons
    sf::Text pauseButton, continueButton;
    pauseButton.setFont(font);
    pauseButton.setCharacterSize(24);
    pauseButton.setFillColor(sf::Color::Red);
    pauseButton.setString("Pause");
    pauseButton.setPosition(1000, 10);
    pauseButton.setStyle(sf::Text::Bold);

    continueButton.setFont(font);
	continueButton.setCharacterSize(24);
	continueButton.setFillColor(sf::Color::Green);
	continueButton.setString("Continue");
	continueButton.setPosition(970, 10);
	
	// Settings screen elements
    sf::Text soundToggleText, musicToggleText, volumeText;
    soundToggleText.setFont(font);
    soundToggleText.setCharacterSize(24);
    soundToggleText.setFillColor(sf::Color::White);
    soundToggleText.setString("Background Music: On");
    soundToggleText.setPosition(440, 150);

    musicToggleText.setFont(font);
    musicToggleText.setCharacterSize(24);
    musicToggleText.setFillColor(sf::Color::White);
    musicToggleText.setString("Helicopter Sound: On");
    musicToggleText.setPosition(445, 200);

    volumeText.setFont(font);
    volumeText.setCharacterSize(24);
    volumeText.setFillColor(sf::Color::White);
    volumeText.setString("Adjust Volume:");
    volumeText.setPosition(450, 250);

    sf::RectangleShape volumeBar(sf::Vector2f(200, 20));
    volumeBar.setFillColor(sf::Color::Blue);
    volumeBar.setPosition(450, 300);

    sf::CircleShape volumeKnob(10);
    volumeKnob.setFillColor(sf::Color::Red);
    volumeKnob.setPosition(450+music.getVolume()*2-10 , 300);
///To here
ScrollingBackground background(100.0f); // Adjust speed as needed
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle game state transitions
           if (gameState == Home && event.type == sf::Event::MouseButtonPressed) 
		{
            if (event.mouseButton.button == sf::Mouse::Left) 
			{
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                if (playButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
                    gameState = LevelSelect;
                } 
				else if (helpButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
                    gameState = Help;
                } 
				else if (aboutButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
                    gameState = About;
                } 
				else if (instructionsButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
                    gameState = Instructions;
                } 
				else if(settingsButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
					gameState = Settings;
				
				}
				else if (exitButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
					Background.stop();
                	window.close(); // Exit the game
                }
            }
          
        }
        if (currentLevel == 3 && gameState == Playing && event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Right) { // Fire a missile with 'right'
        float missileX = helicopter.sprite.getPosition().x + helicopter.sprite.getGlobalBounds().width;
        float missileY = helicopter.sprite.getPosition().y + helicopter.sprite.getGlobalBounds().height / 2 - 5;
        missiles.emplace_back(missileTexture, missileX, missileY);
        missileSound.play(); // Play missile firing sound (if available)
    }
}


            if (gameState == LevelSelect && event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (easyButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        currentLevel = 1; // Easy Level
                        gameState = Playing;
                        elapsedTime = 0;
                        helicopter = Helicopter(helicopterTextureFiles);
                        obstacles.clear();
                        coins.clear();
                        enemyHelicopters.clear();
                        music.play();
                    } else if (mediumButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        currentLevel = 2; // Medium Level
                        gameState = Playing;
                        elapsedTime = 0;
                        helicopter = Helicopter(helicopterTextureFiles);
                        obstacles.clear();
                        coins.clear();
                        enemyHelicopters.clear();
                        music.play();
                    }
                    else if(hardButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                    	currentLevel = 3; // Medium Level
                        gameState = Playing;
                        elapsedTime = 0;
                        helicopter = Helicopter(helicopterTextureFiles); // Reset helicopter position
                        obstacles.clear();
                        coins.clear();
                        enemyHelicopters.clear();
                        
                        music.play();
					}
                }
            }
            if ((gameState == Help || gameState == About || gameState == Instructions||gameState == Settings||gameState==GameOver || gameState==LevelSelect) && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                if (backButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
				{
                    gameState = Home;     
                }
            }
        }
            // Handle pause/continue logic
            if (gameState == Playing || gameState == Paused) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    if (pauseButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y) && gameState == Playing) {
                        gameState = Paused;
                        music.pause(); // Pause music
                        isPaused = true;
                    } 
					else if (continueButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y) && gameState == Paused) 
					{
                        gameState = Playing;
                        music.play(); // Resume music
                        isPaused = false;
                    }
                }
            }
        }
            if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P && gameState == Playing) {
                gameState = Paused;
                music.pause(); // Pause music
                isPaused = true;
            } 
			else if (event.key.code == sf::Keyboard::C && gameState == Paused) {
                gameState = Playing;
                music.play(); // Resume music
                isPaused = false;
            }
        }
            if(gameState==Settings)
           {
        	if (event.type == sf::Event::MouseButtonPressed)
			{
			
        		if (event.mouseButton.button == sf::Mouse::Left)
				{
			
        			sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        			if (soundToggleText.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
						{
                            soundEnabled = !soundEnabled;
                            soundToggleText.setString(soundEnabled ? "Background Music: On" : "Background Music: Off");
                            Background.setVolume(soundEnabled ? 100 : 0);
                    	} 
						else if (musicToggleText.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
						{
                            musicEnabled = !musicEnabled;
                            musicToggleText.setString(musicEnabled ? "Helicopter Sound: On" : "Helicopter Sound: Off");
                            music.setVolume(musicEnabled ? 100 : 0);   
                        } 
						else if (volumeBar.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
						{
                            float newVolume = (mousePosition.x - 450) / 2;
                            if (newVolume < 0) newVolume = 0;
                            if (newVolume > 100) newVolume = 100;
                            music.setVolume(newVolume);
                            volumeKnob.setPosition(450 + newVolume * 2 - 10, 295);
                        }
                       else if (backButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
						{
                            gameState = Home;
                        }
               }     
			}
		}
            if (gameState == GameOver && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                if (playAgainText.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) {
                    // Restart the game
                    gameState = Playing;
                    elapsedTime = 0;
                    obstacles.clear();
                    helicopter = Helicopter(helicopterTextureFiles); // Reset helicopter position
                    enemyHelicopters.clear();
                    missiles.clear();
                    music.play(); // Restart music
                }
            }
        }
      }
//loop ends here
        float deltaTime = clock.restart().asSeconds();
        if (!fadeInComplete && gameState == Home) {
            alpha += fadeSpeed * deltaTime;
            if (alpha >= 255) {
                alpha = 255;
                fadeInComplete = true;
            }
            // Apply the fade effect by changing button colors
            playButton.setFillColor(sf::Color(255, 255, 0, alpha));
            helpButton.setFillColor(sf::Color(255, 255, 0, alpha));
            aboutButton.setFillColor(sf::Color(255, 255, 0, alpha));
            instructionsButton.setFillColor(sf::Color(255, 255, 0, alpha));
            settingsButton.setFillColor(sf::Color(255, 255, 0, alpha));
            exitButton.setFillColor(sf::Color(255, 255, 0, alpha));
        }
        if (gameState == Playing && !isPaused) 
	    {
          background.update(deltaTime);  // Update background only when the game is playing
        }
         
        if (gameState == Intro) 
	    {
          introTime += deltaTime;
          loadingAnimationTimer += deltaTime;

         if (loadingAnimationTimer >= 1.5f) { // Change loading frame every 1.5 seconds
            loadingAnimationTimer = 0;
            currentLoadingFrame = (currentLoadingFrame + 1) % loadingSprites.size();
         }

          if (introTime >= 6.0f || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { // Show intro for 6 seconds or skip with space key
            gameState = Home;
            Background.play();	
          }
       }
        else if (gameState == Playing && !isPaused) {
            elapsedTime += deltaTime;
            // Helicopter update
            helicopter.update(deltaTime);
            // Obstacle logic
            obstacleSpawnTimer += deltaTime;
             if (obstacleSpawnTimer >= obstacleSpawnInterval) 
			 {
                obstacleSpawnTimer = 0;
                float gapY = rand() % 400 + 100;
                obstacles.push_back(Obstacle(1080, gapY));
             }

            for (auto& obstacle : obstacles)
			 {
                obstacle.update(deltaTime);
            }

            // Coin spawning and updating for Medium Level
            if (currentLevel >= 2 ) {
                coinSpawnTimer += deltaTime;
                if (coinSpawnTimer >= coinSpawnInterval) {
                    coinSpawnTimer = 0;
                    float coinY = rand() % 500 + 50; // Random Y position
                    coins.emplace_back(coinTexture, 1080, coinY);
                }

                for (auto it = coins.begin(); it != coins.end();) {
                    it->update(deltaTime);
                    if (helicopter.getBounds().intersects(it->getBounds())) {
                        elapsedTime += 2; // Add 10 points (2 seconds of score)
                        it = coins.erase(it);
                        coinSound.play();
                    } else if (it->sprite.getPosition().x < -50) {
                        it = coins.erase(it); // Remove off-screen coins
                    } else {
                        ++it;
                    }
                }
            }
             // Spawn and update enemy helicopters for Hard level
            if (currentLevel == 3) {
                enemySpawnTimer += deltaTime;
                if (enemySpawnTimer >= enemySpawnInterval) {
                    enemySpawnTimer = 0;
                    float enemyY = rand() % 500 + 50;
                    enemyHelicopters.emplace_back(enemyTextures, 1080, enemyY);
                }
                for (auto it = enemyHelicopters.begin(); it != enemyHelicopters.end();) {
                    it->update(deltaTime);
                    if (helicopter.getBounds().intersects(it->getBounds())) {
                        gameState = Blast;
                        music.stop();
                        blastSound.play();
                        blastTimer = 0;
                        break;
                    } else if (it->sprite.getPosition().x < -50) {
                        it = enemyHelicopters.erase(it);
                    } else {
                        ++it;
                    }
                }
                for (auto it = missiles.begin(); it != missiles.end();) {
    it->update(deltaTime);

    bool missileRemoved = false;

    // Check for collisions with enemy helicopters
    for (auto enemyIt = enemyHelicopters.begin(); enemyIt != enemyHelicopters.end();) {
        if (it->getBounds().intersects(enemyIt->sprite.getGlobalBounds())) {
            enemyIt = enemyHelicopters.erase(enemyIt); // Destroy the enemy
            it = missiles.erase(it); // Destroy the missile
            missileRemoved = true;
            break;
        } else {
            ++enemyIt;
        }
    }

    if (missileRemoved) {
        continue;
    }

    // Remove missiles that go off-screen
    if (it->sprite.getPosition().x > 1080) {
        it = missiles.erase(it);
    } else {
        ++it;
    }
}

            }
            // Collision detection for obstacles
        sf::FloatRect helicopterBounds = helicopter.getBounds();
        for (const auto& obstacle : obstacles) 
		{
            if (helicopterBounds.intersects(obstacle.getTopBounds()) || helicopterBounds.intersects(obstacle.getBottomBounds())) 
			{
                gameState = Blast;
                music.stop(); // Stop music
                blastSound.play(); // Play blast sound
                blastTimer = 0;
                break;
            }
        }
            // Collision with screen edges
            if (helicopterBounds.top <= 0 || helicopterBounds.top + helicopterBounds.height >= 640) {
            gameState = Blast;
            music.stop(); // Stop music
            blastSound.play(); // Play blast sound
            blastTimer = 0;
            }
            // Update score
        int score = static_cast<int>(elapsedTime * 5); // 1 score per 1/5 second
        std::stringstream ss;
        ss << "Score: " << score;
        scoreText.setString(ss.str());
    }
    else if (gameState == Blast) 
	{
        blastTimer += deltaTime;
        if (blastTimer >= 4.5f) 
		{
            gameState = GameOver;
        }
    }
	else if (gameState == GameOver) 
	{
        // Update high score if needed
        int score = static_cast<int>(elapsedTime * 5); // 1 score per 1/5 second
        if (score > highScore) {
            highScore = score;
        }

        // Update popup text
        std::stringstream currentScoreStream;
        currentScoreStream << "Your Score: " << score;
        currentScoreText.setString(currentScoreStream.str());

        std::stringstream highScoreStream;
        highScoreStream << "High Score: " << highScore;
        highScoreText.setString(highScoreStream.str());
    
    }

    window.clear();
        if (gameState == Intro) {
        window.draw(introSprite);
        sf::Sprite& loadingSprite = loadingSprites[currentLoadingFrame];
        loadingSprite.setPosition((1080 - loadingSprite.getGlobalBounds().width) / 2, 550); // Centered at the bottom
        window.draw(loadingSprite);
        window.draw(loadingText);
    } 
	else if (gameState == Home) {
    	
        window.draw(homeSprite);
        window.draw(playButton);
        window.draw(helpButton);
        window.draw(aboutButton);
        window.draw(settingsButton);
        window.draw(instructionsButton);
        window.draw(exitButton);
    } else if (gameState == Instructions) {
        window.draw(instructionsText);
        window.draw(backButton);
    } else if (gameState == Help) {
        window.draw(helpText);
        window.draw(backButton);
    } else if (gameState == About) {
        window.draw(aboutText);
        window.draw(backButton);
    } 
	else if(gameState==Settings)
    {			window.draw(soundToggleText);
                window.draw(musicToggleText);
                window.draw(volumeText);
                window.draw(volumeBar);
                window.draw(volumeKnob);
                window.draw(backButton);    			
	}
	else if(gameState==LevelSelect)
	{
	 
		window.draw(levelSprite);
			window.draw(backButton);
	}
	else if (gameState == Playing || gameState == Paused || gameState == Blast) { // Draw the game field during Blast state
       
		background.render(window);  // Draw the background image
		window.draw(helicopter.sprite); 
        for (const auto& obstacle : obstacles) {
            window.draw(obstacle.topPipe);
            window.draw(obstacle.bottomPipe);
        }
        window.draw(scoreText); // Draw the score text
        if (gameState == Paused) {
            window.draw(continueButton);
            
        } else if (gameState == Playing) {
            window.draw(pauseButton);
        }
// Draw coins for Medium Level
            if (currentLevel == 2||currentLevel == 3 ) {
                for (const auto& coin : coins) {
                    window.draw(coin.sprite);
                }
            }
            if (currentLevel == 3 ) {
            	for (const auto& enemy : enemyHelicopters) window.draw(enemy.sprite);
            	for (const auto& missile : missiles) {
    window.draw(missile.sprite);
}

            }
        if (gameState == Blast) {
            if (blastTimer < 1.0f) {
                blastSprite1.setPosition(helicopter.sprite.getPosition());
                window.draw(blastSprite1);
            } else {
                blastSprite2.setPosition(helicopter.sprite.getPosition());
                window.draw(blastSprite2);
            }
        }
        Background.stop();
       }
    
        if (gameState == LevelSelect) {
    window.draw(easyButton);
    window.draw(mediumButton);
    window.draw(hardButton);
    }
     else if (gameState == GameOver) {
        window.draw(gameOverText);
        window.draw(currentScoreText);
        window.draw(highScoreText);
        window.draw(playAgainText);
        window.draw(backButton);
    }

        window.display();
    }

    return 0;
}

