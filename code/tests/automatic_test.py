import subprocess
import unittest
import os
import tempfile

class TestTokenization(unittest.TestCase):

    def test_tokenization(self):
        # Replace 'your_c_program' with the actual name of your compiled C program
        c_program = '.././out/compo'
        
        # Input text for testing
        bold_text = '**BOLD Text**'
        italic_text = '*ITALIC Text*'
        underline_text = '__UNDERLINE Text__'
        strikethrough_text = '~~STRIKETHROUGH Text~~'
        inline_text = '`INLINE_CODE Text`'
        image = '![Whiterhinos](https://freeimages.com/or/7bab/africa_white_rhinos_rhino.jpg)'
        link = 'Click on [thislink](https://youtu.be/dQw4w9WgXcQ)'

        input_text = bold_text+'\n'+italic_text+underline_text+'\n'+strikethrough_text+'\n'+inline_text+'\n'+image+'\n'+link+' ' # Replace with your actual input text

        # Create a temporary file and write the input text to it
        with tempfile.NamedTemporaryFile(mode='w+', delete=False) as temp_file:
            temp_file.write(input_text)
            temp_file_path = temp_file.name
        
        try:
            # Run the C program and capture the output
            result = subprocess.run([c_program, temp_file_path], capture_output=True, text=True)
         
            # Define the expected tokens for the given input
            bold_expected_token =           'TOKEN: BOLD\nTOKEN: TEXT | String: "BOLD Text"\nTOKEN: BOLD'
            italic_expected_token =         'TOKEN: ITALIC\nTOKEN: TEXT | String: "ITALIC Text"\nTOKEN: ITALIC'
            underline_expected_token =      'TOKEN: UNDERLINE\nTOKEN: TEXT | String: "UNDERLINE Text"\nTOKEN: UNDERLINE'
            strikethrough_expected_token =  'TOKEN: STRIKETHROUGH\nTOKEN: TEXT | String: "STRIKETHROUGH Text"\nTOKEN: STRIKETHROUGH'
            inline_expected_token =         'TOKEN: INLINE_CODE\nTOKEN: TEXT | String: "NLINE_CODE Text"\nTOKEN: INLINE_CODE'
            image_expected_token =          'TOKEN: EXCLAM LBRACKET\nTOKEN: TEXT | String: "Whiterhinos"\nTOKEN: RBRACKET LPAREN TEXT RPAREN'
            link_expected_token =           'TOKEN: TEXT | String: "Click on "\nTOKEN: LBRACKET\nTOKEN: TEXT | String: "hislink"\nTOKEN: RBRACKET LPAREN TEXT RPAREN'

            assert bold_expected_token.strip() in result.stdout.strip(), f"Substring '{bold_expected_token}' not found in the result"
            assert italic_expected_token.strip() in result.stdout.strip(), f"Substring '{italic_expected_token}' not found in the result"
            assert underline_expected_token.strip() in result.stdout.strip(), f"Substring '{underline_expected_token}' not found in the result"
            assert strikethrough_expected_token.strip() in result.stdout.strip(), f"Substring '{strikethrough_expected_token}' not found in the result"
            assert inline_expected_token.strip() in result.stdout.strip(), f"Substring '{inline_expected_token}' not found in the result"
            assert image_expected_token.strip() in result.stdout.strip(), f"Substring '{image_expected_token}' not found in the result"
            assert link_expected_token.strip() in result.stdout.strip(), f"Substring '{link_expected_token}' not found in the result"
        finally:
            # Remove the temporary file
            os.remove(temp_file_path)

if __name__ == '__main__':
    unittest.main()
