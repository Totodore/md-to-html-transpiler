# Markdown Cheat Sheet

Thanks for visiting [The Markdown Guide](https://www.markdownguide.org)!

This Markdown cheat sheet provides a quick overview of all the Markdown syntax elements. It can’t cover every edge case, so if you need more information about any of these elements, refer to the reference guides for [basic syntax](https://www.markdownguide.org/basic-syntax/) and [extended syntax](https://www.markdownguide.org/extended-syntax/).

## Basic Syntax

These are the elements outlined in John Gruber’s original design document. All Markdown applications support these elements.

### Heading

# H1
## H2
### H3

### Bold

**bold text**

### Italic

*italicized text*

### Blockquote

> blockquote

### Code

`code`

### Horizontal Rule

---

### Link

[Markdown Guide](https://www.markdownguide.org)

### Image

![alt text](https://www.markdownguide.org/assets/images/tux.png)

## Extended Syntax

These elements extend the basic syntax by adding additional features. Not all Markdown applications support these elements.

### Fenced Code Block

```
{
  "firstName": "John",
  "lastName": "Smith",
  "age": 25
}
```

### Heading ID

### My Great Heading

### Definition List

term
: definition

### Strikethrough

~~The world is flat.~~

```xsvg:0,0,100,100
line 0,0 10,0 red

line 10,0 10,10 green

polyline 20,0 25,0 25,5 20,5 blue
polyline 30,0 35,0 35,5 30,5 30,10 35,10 35,15 30,15 red

polygon 40,0 45,0 45,5 40,5 blue red

circle 50,5 5 blue red

ellipse 60,5 5 10 blue red

rect 70,0 10 10 blue red

text 0,50 "My Super text" middle red

```
