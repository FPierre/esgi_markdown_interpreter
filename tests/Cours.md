## Bases

Utilisé pour :

* l'imagerie, 3D, CAO

* jeux vidéos

* fabrication d'autres langages ou technologies (moteurs JS, interpréteurs de DB, MySQL, MongoDB)

* gros produits informatique

* embarqué

> Définition selon Stroustrup : noyau de C + transformations et modifications + OOP + programmation générique + programmation fonctionnelle

### Versions

* 1979 : 1ère version, créée par Bjarne Stroustrup, (surcouche objet au C, nommée : C with classes). Ajout des classes, protection, héritage, droits, etc

* 1984 : CFront. Ajout du polymorphisme et surcharge d'opérateurs

* 1998 : 1ère norme ISO du langage. Ajout des templates

* 2011 : C++11. Ajout du fonctionnel

### Différences entre C et C++

### Bibliothèque standard

| C | C++ |
|:-:|:---:|
| stdio.h  | cstdio.h  |
| stdlib.h | cstdlib.h |
| string.h | cstring.h |

C++ :

* fstream

* string

* iostream

* etc...

### Types de chaines

* `char *` (type C, défini dans cstring.h)

* `string s = "hello";` (type C++, à utiliser dès que possible, défini dans string.h. Prend en charge la surcharge d'opérateur)

### Namespace

> Définition : ensemble logique de noms (déclarations) soit de variables globales, soit de fonctions, soit de classes, soit de structures

Exemple : une série d'includes utilisent string. Utiliser string dans main() donnerais une erreur de compilation (ne sachant quel string utiliser)

`using namespace X;` : pour tout ce qui est défini dans ce namespace, plus besoin de préfixer (par défaut std)

#### Déclaration

```c++
// foo.h

namespace X {
    int i;    // Uniquement des variables globales
    void f(); // Prototypes
    struct nom {
        // ...
    };
}
```

Si définition de namespace dans différents .h, le compilateur les fusionnes et évite le problème cité.

#### Utilisation

* `X::i;`

* `void X::f() {}`

* etc...

Exemple :

```c++
namespace X {
    int i = 15;
}
// ...
int i = 17;
// ...
using namespace X;

// `i` sera celui du namespace X
// `::i` sera celui en dehors
```

### Déclaration des variables

C travail en :

* valeur

* pointeur

C++ travail en :

* valeur

* pointeur

* référence (`int &r = n;` : r est une référence (alias, même emplacement mémoire et valeur) sur la variable n)

Conséquences des déclarations par référence :

```c++
void swap(float &a, float &b) {
    float c;

    c = a;
    a = b;
    b = c;
}

int main(void) {
    float x = 1, y = 2;

    swap(x, y);
}
```

> Reconnaitre le & référence et le & adresse :

```c++
type &n; // Passage par référence
&n;      // Adresse
```

La fonction doit instancier un string pour le paramètre (début) et le détruire (fin) : couteux en mémoire

```c++
void f(string s) {}

int main(void) {
    string s = "text";

    f(s);
}
```

Pour résoudre ce problème, passer le paramètre par référence `void f(string &s)`.
Problème, ça modifie la référence partout. Le mot clé `final` permet d'utiliser la référence en interdisant sa modification

```c++
char tab[20] = "bonjour";

const char *p = tab;

*p = 'B'; // Interdit
```

Problème : qu'est ce qui est constant ? L'adresse de tab ? La valeur de tab ?
Réponse : l'adresse (interdiction de la faire sur la valeur)

```c++
class X {
    // Valeur de retour constante, paramètre constant, dernier const rend this constant
    const char *f(const char *p) const
};
```

## C++11

```c++
// Déduit le type et initialise
// auto variable = expression;

int j = 2;

auto i = 2 + j; // i sera un int avec une valeur 4

// Déduit le type sans initialiser
// decltype(expression) variable;

decltype(3.14) x; // x est un double, mais n'a pas la valeur 3.14
```

## Fonctions

* Surcharge

* Arguments par défaut (dans le prototype)

* Fonctions "inline" : `inline int f(); // Prototype`. Gagne du temps à l'exécution, car ne fait pas les allocations et destructions des paramètres de la fonction. Demande au compilateur. Nécessite l'option -o de g++.

* Allocation

`malloc` et `free` sont interdits en C

```c++
int nb = 10;
float *p;

p = new float;
delete p;

// Ou

p = new float[nb];
delete[] p;
```

Tester si les allocations sont réussies

```c++
int *p;

try {
    p = new int[500000];
}
catch (bad_alloc c) {
    cout << e.what();
}

// Ou

p = new (nothrow) int[500000];

// C++ < 11 : NULL
if (p == nullptr) {
    // ...
}
```

### Nouveaux types

* bool (true et false)

* string

```c++
// Initialisation
string s = "foo";

// Longueur
s.length(); // ou s.size();

// Accès à un caratère (sans gestion de débordement de capacité)
s[i];

// Accès à un caractère (avec gestion de débordement de capacité)
s.at(i);

// Sous-chaine
s.substr(begin, length);

// Cherche une sous-chaine
// Retourne position si trouvé, -1 sinon
s.find(needle, begin);
```

* I/O standard

 2 variables globales :

 * `cout` (travail sur l'écran) `cout << expr1 << expr2 << ... << epxrn;`. Inclure iomanip pour avoir accès aux manipulateurs (`endl`, `setw`, `setprecision(nb)`)

 * `cin` (travail sur le clavier) `cin >> v1 >> v2 >> ... >> vn;`

* I/O fichier

 * ifstream (lecture). Mode `ios::int`

 * ofstream (création, écriture). Mode `ios::out`

 * fstream (lecture, écriture, sans création). Mode `ios::binary`

 * Autres modes :

   * Ajout en début de fichier : mode `ios:ate`

   * Ajout en fin de fichier : mode `ios::app`

```c++
fichier("path/file", "mode");

if (!fichier) // Existance du fichier

fichier.close(); // Fermeture
```

Exemple : taille d'une fichier

```c++
ifstream f("file.txt");
f.seekg(0, ios::end);

long taille = f.tellg();

f.close();
```

Exemple : somme des réels d'un fichier

```c++
ifstream("file.txt", ios::binary);

double sum = 0;
double number;

// eof se déclenche 1 case mémoire après la fin de fichier
// Dans un while, seul le dernier sert de condition de validité
while (f.read((char *)&nb, sizeof(double)), !f.eof()) {
    sum += nb;
}

cout << sum << endl;
f.close();
```
## Classes

### Fabrication

1. Construction dans classe (.h)

```c++
class X {
    private:
        // ...
    public:
        // ...
};
```

2. Construction des méthodes (.cpp)

```c++
#include "f.h"

type X::idf(...) {
    // ...
}
```

> Constructeurs :

    * trivial/défaut : celui par défaut si rien crée par le développeur

    * ? : celui qui est crée par le développeur

    * par copie : passage d'un objet dans les paramètres d'une fonction

Constructeur par copie :

    * passage d'un objet dans les paramètres d'une fonction

    * `return Object;`

    * ```c++
      X o;
      X p(o); // ou X p = o;
      ```

Exemple constructeur par copie :

```c++
class ArrayList {
    int size;
    int *array;

    public:
    ArrayList(const ArrayList&);
};

// Constructeur par copie
ArrayList::ArrayList(const ArrayList& al) {
    if (al.size < 0) {
        size = 0;
    }
    else {
        size = al.size;
    }

    if (al.array != nullptr && size > 0) {
        array = new (nothrow) int[int];

        if (array != nullptr) {
            for (int i = 0; i < size; i++) {
                array[i] = al.array[i];
            }
        }
        else {
            size = 0;
        }
    }
    else {
        array = nullptr;
    }
}
```

```c++
class ArrayList {
    int size = 0;
    int *array = nullptr;

    ArrayList(int, int *);

    // C11 : génère un constructeur vide, grâce aux initialisations précédentes
    ArrayList() = default;
}

// Deviens possible d'appeler
ArrayList a;
```

### Propriétés générales

#### Méthodes

> this est un pointeur d'autoréférence

```c++
// Comparaison de 2 ArrayList
// &cmp car pas besoin de passer par le constructeur, l'objet existe déjà
ArrayList &cmp(ArrayList &o) {
    // *this car la fonction retourne un objet
    return this->size < 0 ? o : *this;
}
```

```c++
X o1 = new X(10);
X o2 = new X[10]; // Appel 10x le constructeur. Correspond à 10 appels de X()
delete[] o2; // Les [] sont importants pour supprimer toutes les instances

```c++
X o;
o = X(); // X() appel un constructeur puis un destructeur. Très couteux en mémoire (objet temporaire)
```

```c++
(X()).f; // Fonction f sur objet temporaire X. Sale, et objet temporaire déclaré automatiquement en constant
```

```c++
class date {
    int j, m, a;
};

class etudiant {
    int code;
    float moy;
    date inscription;

    public:
    etudiant(int, float, int, int, int);
};

// Constructeur de class date est appelé
// Donc l'écriture suivate est possible
etudiant::etudiant(int newCode, float newMoy, int newJ, int newM, int newA):inscription(j, m, a) {
    code = newCode;
    moyenne = newMoy;
    // jour = newJ impossible, attributs de la class date sont private par défaut.
    // Possibilité d'utiliser des getters
}
```

### Static

```c++
// .h
class X {
    static type donnee;
    static type idf();
}

// .cpp
type X::idf() {

}

// main.cpp
type X::donnee = valeur;

int main() {
    X::donnee
    X::idf()
}
```
