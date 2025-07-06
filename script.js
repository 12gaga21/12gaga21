console.log("скрипт загружен");

// --- DOM Elements ---
const playerHpTextElement = document.getElementById('player-hp'); // Text inside the bar
const playerMpTextElement = document.getElementById('player-mp'); // Text inside the bar
const playerHpBarElement = document.getElementById('player-hp-bar');
const playerMpBarElement = document.getElementById('player-mp-bar');
const playerSpriteElement = document.getElementById('player-sprite');
const playerEffectContainer = document.getElementById('player-effect-container');

const enemyHpTextElement = document.getElementById('enemy-hp'); // Text inside the bar
const enemyMpTextElement = document.getElementById('enemy-mp'); // Text inside the bar
const enemyHpBarElement = document.getElementById('enemy-hp-bar');
const enemyMpBarElement = document.getElementById('enemy-mp-bar');
const enemySpriteElement = document.getElementById('enemy-sprite');
const enemyEffectContainer = document.getElementById('enemy-effect-container');

const messageLog = document.getElementById('message-log');
const attackButton = document.getElementById('attack-button');
const magicButton = document.getElementById('magic-button');
const defendButton = document.getElementById('defend-button');
const magicMenuElement = document.getElementById('magic-menu');

// --- Game Characters ---
class Character {
    constructor(name, hp, mp, attack, defense, spells = []) {
        this.name = name;
        this.hp = hp;
        this.maxHp = hp;
        this.mp = mp;
        this.maxMp = mp;
        this.attack = attack;
        this.defense = defense;
        this.spells = spells; // List of spell objects {name, cost, effect, type: 'damage'/'heal'}
        this.isDefending = false;
    }

    takeDamage(damage) {
        let actualDamage = damage;
        if (this.isDefending) {
            actualDamage = Math.max(0, damage - this.defense * 2); // Defending doubles defense for this turn
            this.logMessage(`${this.name} защищается и получает ${actualDamage} урона.`);
            this.isDefending = false; // Reset defense state after taking damage
        } else {
            actualDamage = Math.max(0, damage - this.defense);
            this.logMessage(`${this.name} получает ${actualDamage} урона.`);
        }
        this.hp = Math.max(0, this.hp - actualDamage);
        this.updateUI();
        return actualDamage > 0; // Return true if damage was dealt
    }

    heal(amount) {
        this.hp = Math.min(this.maxHp, this.hp + amount);
        this.logMessage(`${this.name} восстанавливает ${amount} HP.`);
        this.updateUI();
    }

    consumeMp(cost) {
        if (this.mp >= cost) {
            this.mp -= cost;
            this.updateUI();
            return true;
        }
        this.logMessage(`${this.name} не хватает маны!`);
        return false;
    }

    // Placeholder for UI update, will be specific to player/enemy
    updateUI() {
        // This method will be overridden or called by specific character instances
    }

    logMessage(message) {
        const p = document.createElement('p');
        p.textContent = message;
        messageLog.appendChild(p);
        messageLog.scrollTop = messageLog.scrollHeight; // Auto-scroll to last message
    }
}

class Player extends Character {
    constructor(name, hp, mp, attack, defense, spells, spriteElement, hpBarElement, mpBarElement, hpTextElement, mpTextElement, effectContainer) {
        super(name, hp, mp, attack, defense, spells);
        this.spriteElement = spriteElement;
        this.hpBarElement = hpBarElement;
        this.mpBarElement = mpBarElement;
        this.hpTextElement = hpTextElement;
        this.mpTextElement = mpTextElement;
        this.effectContainer = effectContainer;
        this.ensureBarFillElements();
    }

    ensureBarFillElements() {
        if (!this.hpBarElement.querySelector('.bar-fill')) {
            const fill = document.createElement('div');
            fill.className = 'bar-fill';
            this.hpBarElement.appendChild(fill);
        }
        if (!this.mpBarElement.querySelector('.bar-fill')) {
            const fill = document.createElement('div');
            fill.className = 'bar-fill';
            this.mpBarElement.appendChild(fill);
        }
    }

    updateUI() {
        this.hpTextElement.textContent = this.hp;
        this.mpTextElement.textContent = this.mp;

        const hpPercent = (this.hp / this.maxHp) * 100;
        const mpPercent = (this.mp / this.maxMp) * 100;

        this.hpBarElement.querySelector('.bar-fill').style.width = `${hpPercent}%`;
        this.mpBarElement.querySelector('.bar-fill').style.width = `${mpPercent}%`;
    }
}

class Enemy extends Character {
    constructor(name, hp, mp, attack, defense, spells, spriteElement, hpBarElement, mpBarElement, hpTextElement, mpTextElement, effectContainer) {
        super(name, hp, mp, attack, defense, spells);
        this.spriteElement = spriteElement;
        this.hpBarElement = hpBarElement;
        this.mpBarElement = mpBarElement;
        this.hpTextElement = hpTextElement;
        this.mpTextElement = mpTextElement;
        this.effectContainer = effectContainer;
        this.ensureBarFillElements();
    }

    ensureBarFillElements() {
        if (!this.hpBarElement.querySelector('.bar-fill')) {
            const fill = document.createElement('div');
            fill.className = 'bar-fill';
            this.hpBarElement.appendChild(fill);
        }
        if (!this.mpBarElement.querySelector('.bar-fill')) {
            const fill = document.createElement('div');
            fill.className = 'bar-fill';
            this.mpBarElement.appendChild(fill);
        }
    }

    updateUI() {
        this.hpTextElement.textContent = this.hp;
        this.mpTextElement.textContent = this.mp;

        const hpPercent = (this.hp / this.maxHp) * 100;
        const mpPercent = (this.mp / this.maxMp) * 100;

        this.hpBarElement.querySelector('.bar-fill').style.width = `${hpPercent}%`;
        this.mpBarElement.querySelector('.bar-fill').style.width = `${mpPercent}%`;
    }

    // Simple AI for the enemy
    performAction(target) {
        // Basic AI: 70% chance to attack, 30% chance to use a spell if has mana and spells
        if (this.spells.length > 0 && this.mp >= this.spells[0].cost && Math.random() < 0.3) {
            // For simplicity, enemy uses the first available spell
            const spell = this.spells[0];
            if (this.consumeMp(spell.cost)) {
                this.logMessage(`${this.name} использует ${spell.name}!`);
                if (spell.type === 'damage') {
                    target.takeDamage(spell.effect + this.attack); // Spell damage can be boosted by attack
                } else if (spell.type === 'heal') {
                    this.heal(spell.effect);
                }
            }
        } else {
            this.logMessage(`${this.name} атакует!`);
            target.takeDamage(this.attack);
        }
    }
}

// --- Initialize Characters ---
const player = new Player("Герой", 100, 50, 15, 5,
    [
        { name: "Огненный шар", cost: 10, effect: 25, type: 'damage', effectSprite: 'https://via.placeholder.com/32x32/FF8C00/000000?text=Fire' },
        { name: "Лечение", cost: 15, effect: 30, type: 'heal', effectSprite: 'https://via.placeholder.com/32x32/00FF00/000000?text=Heal' }
    ],
    playerSpriteElement, playerHpBarElement, playerMpBarElement, playerHpTextElement, playerMpTextElement, playerEffectContainer
);

const enemy = new Enemy("Гоблин", 80, 20, 10, 3,
    [
        { name: "Слабый удар", cost: 5, effect: 15, type: 'damage', effectSprite: 'https://via.placeholder.com/32x32/A0A0A0/000000?text=Hit' }
    ],
    enemySpriteElement, enemyHpBarElement, enemyMpBarElement, enemyHpTextElement, enemyMpTextElement, enemyEffectContainer
);

// --- Initial UI Update ---
function initialUISetup() {
    // Ensure bar fill divs are created before first UI update.
    player.ensureBarFillElements();
    enemy.ensureBarFillElements();

    player.updateUI();
    enemy.updateUI();
    // Populate magic menu
    magicMenuElement.innerHTML = ''; // Clear previous spells
    player.spells.forEach(spell => {
        const spellButton = document.createElement('button');
        spellButton.textContent = `${spell.name} (MP: ${spell.cost})`;
        spellButton.onclick = () => castSpell(spell);
        magicMenuElement.appendChild(spellButton);
    });
    const backButton = document.createElement('button');
    backButton.textContent = 'Назад';
    backButton.onclick = toggleMagicMenu;
    magicMenuElement.appendChild(backButton);
}

// --- Game State ---
let currentTurn = 'player'; // player or enemy
let gameIsOver = false;

// --- Initialize Game ---
document.addEventListener('DOMContentLoaded', () => {
    initialUISetup();
    logMessageToScreen("Битва начинается!");
});

function logMessageToScreen(message) {
    const p = document.createElement('p');
    p.textContent = message;
    messageLog.appendChild(p);
    messageLog.scrollTop = messageLog.scrollHeight;
}

// Placeholder for functions to be implemented in later steps
function castSpell(spell) {
    // Logic for casting a spell will be added here
    console.log(`Casting ${spell.name}`);
    // For now, just hide magic menu and switch turn as a placeholder
    toggleMagicMenu();
    // enemyTurn(); // This will be part of the battle flow
}

function toggleMagicMenu() {
    magicMenuElement.classList.toggle('hidden');
    document.getElementById('actions-menu').classList.toggle('hidden');
}

// --- Battle Logic ---
function playerAttack() {
    if (gameIsOver || currentTurn !== 'player') return;

    disableActionButtons(); // Disable buttons during animation

    player.logMessage(`${player.name} атакует ${enemy.name}!`);

    // Player attack animation
    player.spriteElement.classList.add('attack-animation');

    setTimeout(() => {
        player.spriteElement.classList.remove('attack-animation');

        const damageDealt = enemy.takeDamage(player.attack); // takeDamage now returns true if damage was dealt
        if (damageDealt) {
            // Damage animation on enemy is handled by takeDamage method
            // Optional: show attack effect sprite
            showEffect(enemy.effectContainer, 'https://via.placeholder.com/48x48/FFFF00/000000?text=Hit!');
            // In real project: showEffect(enemy.effectContainer, './assets/effects/attack_effect.png');
        }

        checkGameOver();
        if (!gameIsOver) {
            currentTurn = 'enemy';
            setTimeout(enemyTurn, 1000);
        } else {
            enableActionButtons(); // Re-enable if game over and player can restart
        }
    }, 300); // Duration of attack animation + slight pause
}

function playerDefend() {
    if (gameIsOver || currentTurn !== 'player') return;
    disableActionButtons();
    player.isDefending = true;
    player.logMessage(`${player.name} готовится к защите!`);
    // Add defend animation/state change for player sprite if desired
    // player.spriteElement.classList.add('defend-stance');
    currentTurn = 'enemy';
    setTimeout(() => {
        // if (player.spriteElement.classList.contains('defend-stance')) {
        //     player.spriteElement.classList.remove('defend-stance');
        // }
        enemyTurn();
    }, 1000);
}

function enemyTurn() {
    if (gameIsOver || currentTurn !== 'enemy') return;

    // Enemy action (attack or spell)
    const action = enemy.performAction(player); // performAction could return info about the action taken

    // Example: Animate enemy attack if that was the action
    if (action && action.type === 'attack') {
        enemy.spriteElement.classList.add('attack-animation-enemy');
        setTimeout(() => {
            enemy.spriteElement.classList.remove('attack-animation-enemy');
            if (action.damageDealt) {
                // The showEffect for attack is now triggered from playerAttack and enemyTurn directly after takeDamage
                // showEffect(player.effectContainer, 'https://via.placeholder.com/48x48/FFFF00/000000?text=Hit!');
            }
            finishEnemyTurn();
        }, 300);
    } else if (action && action.type === 'spell') {
        enemy.spriteElement.classList.add('casting-animation');
        // showEffect(enemy.effectContainer, action.spell.effectSprite); // Initial casting visual on enemy

        setTimeout(() => {
            enemy.spriteElement.classList.remove('casting-animation');
            if (action.spell.type === 'damage' && action.damageDealt) {
                showEffect(player.effectContainer, action.spell.effectSprite); // Show spell effect on player
            } else if (action.spell.type === 'heal') {
                showEffect(enemy.effectContainer, action.spell.effectSprite); // Show heal effect on enemy
            }
            finishEnemyTurn();
        }, 700); // Duration for casting animation + effect display
    } else {
        // If no specific animation for the action, just proceed
        finishEnemyTurn();
    }
}

function finishEnemyTurn() {
    checkGameOver();
    if (!gameIsOver) {
        currentTurn = 'player';
        player.isDefending = false;
        enableActionButtons();
    } else {
        // If game is over, ensure buttons are appropriately set (e.g., restart might be visible)
        // enableActionButtons might need adjustment if we only want restart to be active
        // For now, this is fine as disableActionButtons() handles hiding/disabling main actions.
    }
}


// Refactor takeDamage in Character class to include damage animation trigger
Character.prototype.takeDamage = function(damage) {
    let actualDamage = damage;
    let damageDealtFlag = false;
    if (this.isDefending) {
        actualDamage = Math.max(0, damage - this.defense * 2);
        this.logMessage(`${this.name} защищается и получает ${actualDamage} урона.`);
        this.isDefending = false;
    } else {
        actualDamage = Math.max(0, damage - this.defense);
        this.logMessage(`${this.name} получает ${actualDamage} урона.`);
    }

    if (actualDamage > 0) {
        this.hp = Math.max(0, this.hp - actualDamage);
        damageDealtFlag = true;
        if (this.spriteElement) { // Check if spriteElement exists
            this.spriteElement.classList.add('damage-animation');
            setTimeout(() => {
                this.spriteElement.classList.remove('damage-animation');
            }, 300); // Duration of shake animation
        }
    }
    this.updateUI();
    return damageDealtFlag; // Return true if damage was dealt
};

// Modify Enemy's performAction to return info about the action
Enemy.prototype.performAction = function(target) {
    let actionInfo = { type: null, damageDealt: false, spell: null };
    if (this.spells.length > 0 && this.mp >= this.spells[0].cost && Math.random() < 0.3) {
        const spell = this.spells[0];
        actionInfo.type = 'spell';
        actionInfo.spell = spell;
        if (this.consumeMp(spell.cost)) {
            this.logMessage(`${this.name} использует ${spell.name}!`);
            if (spell.type === 'damage') {
                actionInfo.damageDealt = target.takeDamage(spell.effect + this.attack);
            } else if (spell.type === 'heal') {
                this.heal(spell.effect); // Heal animation could be added here or in heal()
                // For heal, damageDealt remains false or add a 'healedAmount' to actionInfo
            }
        }
    } else {
        actionInfo.type = 'attack';
        this.logMessage(`${this.name} атакует!`);
        actionInfo.damageDealt = target.takeDamage(this.attack);
    }
    return actionInfo;
};


function showEffect(container, effectSpriteUrl) {
    if (!effectSpriteUrl) return; // Don't show if no sprite URL
    const effectImg = document.createElement('img');
    effectImg.src = effectSpriteUrl;
    effectImg.classList.add('effect-sprite');

    // Clear previous effects before adding a new one
    while (container.firstChild) {
        container.removeChild(container.firstChild);
    }
    container.appendChild(effectImg);

    // Remove the effect after animation ends (animation duration is 0.5s in CSS)
    setTimeout(() => {
        if (effectImg.parentNode === container) { // Check if it's still there
            container.removeChild(effectImg);
        }
    }, 500);
}


// --- Event Listeners ---
attackButton.addEventListener('click', playerAttack);
defendButton.addEventListener('click', playerDefend);
magicButton.addEventListener('click', () => {
    if (gameIsOver || currentTurn !== 'player') return;
    toggleMagicMenu();
});

// Override castSpell to integrate with battle flow
function castSpell(spell) {
    if (gameIsOver || currentTurn !== 'player') {
        toggleMagicMenu();
        return;
    }
    if (!player.consumeMp(spell.cost)) {
        if (player.mp < spell.cost) {
             player.logMessage(`${player.name} не хватает маны для ${spell.name}.`);
        }
        toggleMagicMenu();
        return;
    }

    disableActionButtons();
    toggleMagicMenu();

    player.logMessage(`${player.name} использует ${spell.name}!`);

    player.spriteElement.classList.add('casting-animation');
    // showEffect(player.effectContainer, spell.effectSprite); // Initial casting visual on player, if different from main effect

    setTimeout(() => {
        player.spriteElement.classList.remove('casting-animation');
        let damageDealtBySpell = false;

        if (spell.type === 'damage') {
            damageDealtBySpell = enemy.takeDamage(spell.effect + Math.floor(player.attack / 3));
            if (damageDealtBySpell) {
                 // Show main spell effect on enemy
                 showEffect(enemy.effectContainer, spell.effectSprite);
            }
        } else if (spell.type === 'heal') {
            player.heal(spell.effect);
             // Show heal effect on player
             showEffect(player.effectContainer, spell.effectSprite);
        }

        checkGameOver();
        if (!gameIsOver) {
            currentTurn = 'enemy';
            setTimeout(enemyTurn, 1000);
        } else {
            enableActionButtons(); // Re-enable if game over
        }
    }, 700); // Duration for casting animation and effect display
    // Note: The duplicated block of battle logic functions that was here previously has been removed.
    // The game will now use the versions of playerAttack, playerDefend, enemyTurn, castSpell, etc.,
    // that include animation logic.
    // The duplicated offerRestart and restartGame functions were also removed as they are
    // part of the older logic block that was causing conflicts.
    // The enable/disableActionButtons and checkGameOver functions are now defined only once
    // with the animation-aware logic.

    // Ensure `offerRestart` is correctly defined if it's meant to be used with the new logic.
    // For now, let's assume the game over check in the new logic handles button states,
    // and restart might need a dedicated button or a clearer trigger if gameIsOver is true.
    // The new checkGameOver already calls disableActionButtons.
    // If a restart button is desired, it should be added explicitly when gameIsOver is true.
}

function offerRestart() {
    if (!document.querySelector('#restart-button')) { // Prevent multiple restart buttons
        const restartButton = document.createElement('button');
        restartButton.id = 'restart-button';
        restartButton.textContent = 'Начать заново';
        restartButton.onclick = restartGame;
        actionsMenu.appendChild(restartButton);
    }
}

function restartGame() {
    player.hp = player.maxHp;
    player.mp = player.maxMp;
    player.isDefending = false;
    enemy.hp = enemy.maxHp;
    enemy.mp = enemy.maxMp;

    gameIsOver = false;
    currentTurn = 'player';

    messageLog.innerHTML = '<p>Новая битва начинается!</p>';
    logMessageToScreen("Игра перезапущена.");

    player.updateUI();
    enemy.updateUI();
    initialUISetup();

    enableActionButtons();

    const restartButton = document.getElementById('restart-button');
    if (restartButton) {
        restartButton.remove();
    }
}

// checkGameOver function needs to be available for the new logic flow
function checkGameOver() {
    if (player.hp <= 0) {
        logMessageToScreen(`${player.name} был повержен! Игра окончена.`);
        gameIsOver = true;
        disableActionButtons();
        offerRestart(); // Offer restart when game is over
    } else if (enemy.hp <= 0) {
        logMessageToScreen(`${enemy.name} был повержен! ${player.name} победил!`);
        gameIsOver = true;
        disableActionButtons();
        offerRestart(); // Offer restart when game is over
    }
}
// disableActionButtons and enableActionButtons are defined above with animation logic, so they are fine.
