"You can't do object-oriented programming in C".

```
gcc main.c game/game.c util/util.c player/player.c player/impl/activist.c player/impl/bear.c player/impl/clairvoyant.c player/impl/healer.c player/impl/townsperson.c -o main -Wall -Werror -fsanitize=address
```