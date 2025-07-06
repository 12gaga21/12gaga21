console.log("скрипт загружен");

// --- DOM Elements ---
const playerHpElement = document.getElementById('player-hp');
const playerMpElement = document.getElementById('player-mp');
const enemyHpElement = document.getElementById('enemy-hp');
const enemyMpElement = document.getElementById('enemy-mp');
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
    constructor(name, hp, mp, attack, defense, spells) {
        super(name, hp, mp, attack, defense, spells);
    }

    updateUI() {
        playerHpElement.textContent = this.hp;
        playerMpElement.textContent = this.mp;
    }
}

class Enemy extends Character {
    constructor(name, hp, mp, attack, defense, spells) {
        super(name, hp, mp, attack, defense, spells);
    }

    updateUI() {
        enemyHpElement.textContent = this.hp;
        enemyMpElement.textContent = this.mp;
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
const player = new Player("Герой", 100, 50, 15, 5, [
    { name: "Огненный шар", cost: 10, effect: 25, type: 'damage' },
    { name: "Лечение", cost: 15, effect: 30, type: 'heal' }
]);

const enemy = new Enemy("Гоблин", 80, 20, 10, 3, [
    { name: "Слабый удар", cost: 5, effect: 15, type: 'damage'}
]);

// --- Initial UI Update ---
function initialUISetup() {
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
    player.logMessage(`${player.name} атакует ${enemy.name}!`);
    enemy.takeDamage(player.attack);
    checkGameOver();
    if (!gameIsOver) {
        currentTurn = 'enemy';
        setTimeout(enemyTurn, 1000); // Simulate enemy thinking time
    }
}

function playerDefend() {
    if (gameIsOver || currentTurn !== 'player') return;
    player.isDefending = true;
    player.logMessage(`${player.name} готовится к защите!`);
    currentTurn = 'enemy';
    setTimeout(enemyTurn, 1000);
}

function enemyTurn() {
    if (gameIsOver || currentTurn !== 'enemy') return;
    enemy.performAction(player); // Enemy AI decides action
    checkGameOver();
    if (!gameIsOver) {
        currentTurn = 'player';
        player.isDefending = false; // Player's defense stance resets at the start of their turn
        enableActionButtons();
    }
}

function checkGameOver() {
    if (player.hp <= 0) {
        logMessageToScreen(`${player.name} был повержен! Игра окончена.`);
        gameIsOver = true;
        disableActionButtons();
    } else if (enemy.hp <= 0) {
        logMessageToScreen(`${enemy.name} был повержен! ${player.name} победил!`);
        gameIsOver = true;
        disableActionButtons();
    }
}

function disableActionButtons() {
    attackButton.disabled = true;
    magicButton.disabled = true;
    defendButton.disabled = true;
    magicMenuElement.classList.add('hidden'); // Hide magic menu too
    document.getElementById('actions-menu').classList.remove('hidden'); // Ensure actions menu is visible if magic was open
}

function enableActionButtons() {
    attackButton.disabled = false;
    magicButton.disabled = false;
    defendButton.disabled = false;
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
    if (gameIsOver || currentTurn !== 'player' || !player.consumeMp(spell.cost)) {
        if (player.mp < spell.cost) {
             player.logMessage(`${player.name} не хватает маны для ${spell.name}.`);
        }
        toggleMagicMenu(); // Hide menu even if spell fails due to mana
        return;
    }

    player.logMessage(`${player.name} использует ${spell.name}!`);
    if (spell.type === 'damage') {
        enemy.takeDamage(spell.effect + Math.floor(player.attack / 3)); // Add a small portion of attack to spell damage
    } else if (spell.type === 'heal') {
        player.heal(spell.effect);
    }

    toggleMagicMenu(); // Hide magic menu after casting
    checkGameOver();
    if (!gameIsOver) {
        currentTurn = 'enemy';
        setTimeout(enemyTurn, 1000);
    } else {
        // Offer to restart the game if it's over
        offerRestart();
    }
}

function offerRestart() {
    const restartButton = document.createElement('button');
    restartButton.textContent = 'Начать заново';
    restartButton.onclick = restartGame;
    // Add it to a suitable place, e.g., action menu or message log
    const actionsMenu = document.getElementById('actions-menu');
    actionsMenu.appendChild(restartButton); // Or messageLog.appendChild(restartButton);
}

function restartGame() {
    // Reset character stats
    player.hp = player.maxHp;
    player.mp = player.maxMp;
    player.isDefending = false;
    enemy.hp = enemy.maxHp; // Assuming enemy has maxHp defined, or reset to initial HP
    enemy.mp = enemy.maxMp; // Assuming enemy has maxMp defined, or reset to initial MP

    // Reset game state
    gameIsOver = false;
    currentTurn = 'player';

    // Clear message log
    messageLog.innerHTML = '<p>Новая битва начинается!</p>';

    // Update UI
    player.updateUI();
    enemy.updateUI();
    initialUISetup(); // Re-populate magic menu in case it was modified or for future features

    // Re-enable action buttons
    enableActionButtons();

    // Remove restart button
    const existingRestartButton = document.querySelector('#actions-menu button:last-child'); // Be more specific if needed
    if (existingRestartButton && existingRestartButton.textContent === 'Начать заново') {
        existingRestartButton.remove();
    }
    logMessageToScreen("Игра перезапущена.");
}
