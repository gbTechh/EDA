#!/usr/bin/env python3
import spacy
import nltk
from nltk.corpus import stopwords
import string
import json
import sys
import re

class EnglishPreprocessor:
    def __init__(self):
        self.puntuacion = set(string.punctuation + '”“‘’…–—')
        
        # Cargar modelo de spaCy para inglés
        try:
            self.nlp = spacy.load('en_core_web_sm')
            self.stopwords = set(stopwords.words('english'))
            
            # Stopwords adicionales específicas para noticias deportivas
            self.sports_stopwords = {
                'game', 'team', 'player', 'season', 'league', 'nfl', 'nba', 'mlb', 
                'nhl', 'point', 'play', 'win', 'loss', 'vs', 'versus', 'year',
                'time', 'day', 'week', 'month', 'first', 'last', 'next', 'back'
            }
            self.stopwords.update(self.sports_stopwords)
            
        except OSError:
            print("Error: English spaCy model not found. Run:")
            print("python -m spacy download en_core_web_sm")
            sys.exit(1)
    
    def clean_text(self, text):
        """Comprehensive text cleaning for English sports news"""
        # Convert to lowercase
        text = text.lower()
        
        # Remove URLs
        text = re.sub(r'http\S+', '', text)
        
        # Remove social media handles and hashtags
        text = re.sub(r'@\w+', '', text)
        text = re.sub(r'#\w+', '', text)
        
        # Keep scores and numbers (important for sports)
        # text = re.sub(r'\b\d+-\d+\b', ' SCORE ', text)  # Keep scores like "3-0"
        # text = re.sub(r'\b\d+\b', ' NUMBER ', text)     # Keep other numbers
        
        # Remove special characters but keep apostrophes for contractions
        text = re.sub(r'[^\w\s\']', ' ', text)
        
        # Handle common sports abbreviations
        text = re.sub(r'\bphiladelphia\b', 'philly', text)
        text = re.sub(r"\bwon't\b", 'will not', text)
        text = re.sub(r"\bcan't\b", 'can not', text)
        text = re.sub(r"\bn't\b", ' not', text)
        text = re.sub(r"'ll", ' will', text)
        text = re.sub(r"'ve", ' have', text)
        text = re.sub(r"'re", ' are', text)
        text = re.sub(r"'s", ' is', text)
        text = re.sub(r"'d", ' would', text)
        
        # Remove extra whitespace
        text = re.sub(r'\s+', ' ', text).strip()
        
        return text
    
    def preprocess_text(self, text):
        """Complete text preprocessing pipeline"""
        if not text or not text.strip():
            return []
        
        # 1. Basic cleaning
        clean_text = self.clean_text(text)
        
        if not clean_text.strip():
            return []
        
        # 2. Process with spaCy for advanced tokenization and lemmatization
        doc = self.nlp(clean_text)
        
        processed_tokens = []
        for token in doc:
            # Filter stopwords, punctuation, spaces, and short tokens
            if (not token.is_stop and 
                not token.is_punct and 
                not token.is_space and
                len(token.lemma_.strip()) > 2 and  # Increased minimum length
                token.lemma_.strip() not in self.stopwords and
                token.lemma_.isalpha()):  # Keep only alphabetic tokens
                
                lemma = token.lemma_.lower().strip()
                processed_tokens.append(lemma)
        
        return processed_tokens

def main():
    if len(sys.argv) > 1:
        text = sys.argv[1]
        preprocessor = EnglishPreprocessor()
        tokens = preprocessor.preprocess_text(text)
        
        # Save as JSON
        with open('/tmp/processed_tokens.json', 'w', encoding='utf-8') as f:
            json.dump(tokens, f, ensure_ascii=False, indent=2)
        
        print(f"Processed: {len(tokens)} tokens")
        # Debug: print tokens
        print("Tokens:", tokens)
    else:
        print("Usage: python preprocesador_english.py \"text to process\"")

if __name__ == "__main__":
    main()