import telebot
from telegram.ext import Updater, CommandHandler, MessageHandler
from telebot.types import InlineKeyboardMarkup, InlineKeyboardButton

import requests

# Home users
user1 = xxxxxxxxx
user2 = xxxxxxxxxx
user3 = xxxxxxxxxx

# Server IPs
door = 'http://[esp32_IP]/open'
ntfy = 'http://[ntfy_IP]/doorBell'

# Create a new bot instance using your bot token
bot = telebot.TeleBot("API_TOKEN")

# Functions
def menu():
    menu = InlineKeyboardMarkup()
    menu.row_width = 1
    menu.add(InlineKeyboardButton('🔓 Open', callback_data='openDoor'))
    return menu

@bot.callback_query_handler(func=lambda call: True)
def callback_query(call):
    if call.data == "openDoor":
        bot.answer_callback_query(call.id, "Puerta abierta. Por favor, cierre después de entrar.")
        try:
            requests.get(door)
            requests.post(ntfy,
                          data='✅ Puerta abierta.')
        except requests.RequestException as e:
            requests.post(ntfy,
                          data='⚠️ Error al abrir la puerta.'.encode('utf-8'))

def start(message, user):
    if user == user1:
        name = "user1"
        chatID = user1
    elif user == user2:
        name = "user2"
        chatID = user2
    elif user == user3:
        name = "user3"
        chatID = user3
    bot.send_message(chatID, f"Hello! <b>{name}</b>", parse_mode="html", reply_markup=menu())

# Define a handler for the /start command
@bot.message_handler(commands=['start'])
def handle_start(message):
    user = message.chat.id
    if message.chat.id == user1 or message.chat.id == user2 or message.chat.id == user3:
        start(message, user)
    else:
        bot.reply_to(message, "Hello! I'm your Telegram bot.")

# Start the bot
bot.polling()