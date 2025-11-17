#!/usr/bin/env python3
import re
import json
import sys
import string

class SimplePreprocessor:
    def __init__(self):
        # Stopwords básicas en inglés
        self.stopwords = {
            'the', 'a', 'an', 'and', 'or', 'but', 'in', 'on', 'at', 'to', 
            'for', 'of', 'with', 'by', 'as', 'is', 'was', 'are', 'were', 'be',
            'been', 'have', 'has', 'had', 'do', 'does', 'did', 'will', 'would',
            'could', 'should', 'this', 'that', 'these', 'those', 'i', 'you', 'he',
            'she', 'it', 'we', 'they', 'my', 'your', 'his', 'her', 'its', 'our',
            'their', 'what', 'which', 'who', 'whom', 'when', 'where', 'why', 'how',
            'all', 'any', 'both', 'each', 'few', 'more', 'most', 'other', 'some',
            'such', 'no', 'nor', 'not', 'only', 'own', 'same', 'so', 'than', 'too',
            'very', 'can', 'just', 'should', 'now', 'game', 'team', 'player', 'season'
        }
        
        # Pre-compilar regex para mayor velocidad
        self.clean_patterns = [
            (re.compile(r'http\S+'), ''),                    # URLs
            (re.compile(r'[@#]\w+'), ''),                    # Menciones y hashtags  
            (re.compile(r'[^\w\s]'), ' '),                   # Caracteres especiales
            (re.compile(r'\d+'), ''),                        # Números
            (re.compile(r'\s+'), ' '),                       # Espacios múltiples
        ]
    
    def clean_text(self, text):
        """Limpieza básica del texto"""
        text = text.lower()
        
        # Aplicar todos los patrones de limpieza
        for pattern, replacement in self.clean_patterns:
            text = pattern.sub(replacement, text)
            
        return text.strip()
    
    def simple_stemming(self, word):
        """Stemming básico (sin librerías externas)"""
        if len(word) <= 3:
            return word
            
        # Reglas básicas de stemming
        if word.endswith('ing'):
            return word[:-3]
        elif word.endswith('ed'):
            return word[:-2] 
        elif word.endswith('s') and len(word) > 3:
            return word[:-1]
        elif word.endswith('ly'):
            return word[:-2]
            
        return word
    
    def preprocess_text(self, text):
        """Procesamiento simplificado sin spaCy"""
        if not text or len(text) < 5:
            return []
        
        # 1. Limpieza básica
        clean_text = self.clean_text(text)
        if not clean_text:
            return []
        
        # 2. Tokenización simple
        tokens = clean_text.split()
        
        # 3. Filtrar y procesar tokens
        processed_tokens = []
        for token in tokens:
            # Filtrar stopwords y tokens cortos
            if (len(token) > 2 and 
                token not in self.stopwords and
                token.isalpha()):
                
                # Aplicar stemming básico
                stemmed_token = self.simple_stemming(token)
                processed_tokens.append(stemmed_token)
        
        return processed_tokens

def main():
    if len(sys.argv) > 1:
        text = sys.argv[1]
        preprocessor = SimplePreprocessor()
        
        tokens = preprocessor.preprocess_text(text)
        
        # Guardar resultados
        with open('/tmp/processed_tokens.json', 'w', encoding='utf-8') as f:
            json.dump(tokens, f, ensure_ascii=False, indent=2)
        
        print(f"Processed: {len(tokens)} tokens")
        
    else:
        print("Usage: python preprocesador_simple.py \"text to process\"")

if __name__ == "__main__":
    main()