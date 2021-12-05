Pour ce projet, j'ai eu pas mal de bloquages pour comprendre exactement quel était le cahier des charges pour le serveur Web.
Je fût légèrement frustré car j'avais l'impression de me battre avec des librairies pas forcément bien documentées, et de perdre un peu mon temps dessus.

Gros respect à la famille du dev de ESP32AsyncWebServer pour sa documentation très bien faite disponible directement sur GitHub. Rien à redire.

Pour le JSON et les codes HTTP par contre, mon expérience chez Citrix m'a déjà appris toutes les bases et plus, donc il n'était pas très compliqué de tester tout ça et de comprendre comment ça marchait.
J'ai configuré un container Grafana sur ma machine locale pour contacter mon ESP32, mais je me suis rendu compte plus tard que Grafana était apparemment incapable de recevoir des requêtes POST, seulement d'en envoyer.

J'ai vraiment hâte de commencer à utiliser MongoDB et une API pour pouvoir utiliser Grafana à son plein potentiel.

Le plus gros de mon travail a été séparé en deux parties : le HTTP dont j'ai parlé au-dessus, mais aussi beaucoup de refactoring de mon code. Lors du TP1, tout mon code était dans mon main.ino. Pour rendre le développement
plus simple et pour pouvoir séparer mon travail un peu plus, j'ai séparé mon code en plusieurs fichiers suivants ce qu'ils font, et j'ai créé le fichier vars.h pour contenir toutes les variables globales que j'utilise
dans mon ESP.

J'ai eu quelques problèmes de scope, mais en utilisant le mot-clé extern tout fût réglé assez rapidement.

Au final, j'ai fait mon dashboard assez rapidement une fois que mon problème de requête POST a été réglé.
En utilisant le JSON, on arrive à naviguer les données bien plus facilement, donc j'espère très fortement qu'on va continuer à l'utiliser pour le TP3.
De toute façon en utilisant MongoDB on ne fera pas autrement.

Je pense concrètement que le TP3 sera plus facile à faire pour moi, car il y aura beaucoup moins de lifting à faire sur mon code. Utiliser MQTT me semble plus adapté car je ne pouvais pas m'empêcher de penser qu'utiliser HTTP
comme je l'ai fait n'était pas la solution optimale et n'était pas scalable.