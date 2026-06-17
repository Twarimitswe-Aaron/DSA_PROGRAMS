/*
 * ============================================================================
 * FILE: 28_music_player.cpp
 * ============================================================================
 * MUSIC PLAYER / PLAYLIST MANAGER - Real-world DSA Application
 *
 * EXAM FREQUENCY: High. Music player is a classic DSA exam problem testing
 * circular linked lists (playlist looping), stacks (recently played),
 * and queues (upcoming songs).
 *
 * DATA STRUCTURES USED:
 *   1. Circular Doubly Linked List - Playlist
 *      - "Next" wraps to first song; "Prev" wraps to last
 *      - Doubly linked for both forward and backward navigation
 *   2. Stack - Recently played / history (LIFO)
 *      - Back button: most recent song first
 *   3. Queue (FIFO) - Upcoming queue / songs to play next
 *      - "Play Next" adds to front of upcoming queue
 *   4. Hash Table (unordered_map) - Song lookup by title/ID
 *      - O(1) search across entire library
 *   5. Min-Heap (priority_queue) - Most played / favorites
 *      - Auto-generated playlist of most frequently played songs
 *
 * FUNCTIONALITIES:
 *   - Add/remove songs from library (hash table)
 *   - Create playlist (circular doubly linked list)
 *   - Play/Pause/Next/Previous (circular list traversal)
 *   - View recently played (stack - most recent first)
 *   - Add to "Play Next" queue (FIFO)
 *   - View most played songs (priority queue / heap)
 *   - Shuffle and repeat modes
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cstdlib>
using namespace std;

// ============================================================================
// SECTION 1: SONG ENTITY
// ============================================================================
struct Song {
    string songId;
    string title;
    string artist;
    string album;
    int duration;       // Duration in seconds
    int playCount;      // Times played (for most-played heap)

    Song() : duration(0), playCount(0) {}
    Song(string id, string t, string a, string alb, int dur)
        : songId(id), title(t), artist(a), album(alb),
          duration(dur), playCount(0) {}

    string getDurationStr() const {
        int min = duration / 60;
        int sec = duration % 60;
        stringstream ss;
        ss << min << ":" << setw(2) << setfill('0') << sec;
        return ss.str();
    }
};

// ============================================================================
// SECTION 2: CIRCULAR DOUBLY LINKED LIST FOR PLAYLIST
//
// Why Circular Doubly Linked List?
//   - "Next" from last song goes back to first (circular)
//   - "Previous" from first goes to last (circular)
//   - Doubly linked for O(1) traversal in both directions
//   - Perfect for music player loop behavior
// ============================================================================
struct PlaylistNode {
    Song song;
    PlaylistNode* prev;  // Previous song in playlist
    PlaylistNode* next;  // Next song in playlist

    PlaylistNode(Song s) : song(s), prev(nullptr), next(nullptr) {}
};

class Playlist {
private:
    PlaylistNode* current;  // Currently playing song
    int count;

public:
    Playlist() : current(nullptr), count(0) {}

    ~Playlist() {
        if (!current) return;
        PlaylistNode* start = current;
        do {
            PlaylistNode* temp = current;
            current = current->next;
            delete temp;
        } while (current != start);
    }

    // -------------------------------------------------------------------
    // ADD SONG: Insert after current position. O(1).
    // -------------------------------------------------------------------
    void addSong(Song s) {
        PlaylistNode* node = new PlaylistNode(s);
        if (!current) {
            current = node;
            current->next = current;
            current->prev = current;
        } else {
            node->next = current->next;
            node->prev = current;
            current->next->prev = node;
            current->next = node;
        }
        count++;
    }

    // -------------------------------------------------------------------
    // REMOVE CURRENT SONG: O(1) in circular doubly linked list.
    // -------------------------------------------------------------------
    bool removeCurrent() {
        if (!current) return false;
        if (count == 1) {
            delete current;
            current = nullptr;
            count = 0;
            return true;
        }
        PlaylistNode* toDelete = current;
        current->prev->next = current->next;
        current->next->prev = current->prev;
        current = current->next;
        delete toDelete;
        count--;
        return true;
    }

    // -------------------------------------------------------------------
    // NEXT: Move to next song in circular list. O(1).
    // -------------------------------------------------------------------
    Song* next() {
        if (!current) return nullptr;
        current = current->next;
        return &current->song;
    }

    // -------------------------------------------------------------------
    // PREVIOUS: Move to previous song in circular list. O(1).
    // -------------------------------------------------------------------
    Song* prev() {
        if (!current) return nullptr;
        current = current->prev;
        return &current->song;
    }

    Song* getCurrent() {
        if (!current) return nullptr;
        return &current->song;
    }

    // -------------------------------------------------------------------
    // DISPLAY PLAYLIST: Traverse circular list. O(n).
    // -------------------------------------------------------------------
    void display() {
        if (!current) {
            cout << "    [Empty Playlist]\n";
            return;
        }
        cout << "    " << left << setw(8) << "Now: "
             << current->song.title << " - " << current->song.artist << "\n";
        cout << "    Playlist (" << count << " songs):\n";
        cout << "    " << left << setw(6) << "#"
             << setw(30) << "Title"
             << setw(25) << "Artist"
             << "Duration\n";
        cout << "    " << string(70, '-') << "\n";

        PlaylistNode* start = current;
        int idx = 0;
        do {
            cout << "    " << left << setw(6) << (idx == 0 ? ">>" : to_string(idx))
                 << setw(30) << current->song.title
                 << setw(25) << current->song.artist
                 << current->song.getDurationStr() << "\n";
            current = current->next;
            idx++;
        } while (current != start);
    }

    int getCount() { return count; }
    bool isEmpty() { return current == nullptr; }
};

// ============================================================================
// SECTION 3: MAIN MUSIC PLAYER SYSTEM
// ============================================================================

// Comparator for most-played heap (max-heap by playCount)
struct ComparePlayCount {
    bool operator()(const Song& a, const Song& b) {
        return a.playCount < b.playCount;
    }
};

class MusicPlayer {
private:
    unordered_map<string, Song> library;        // Hash: songId -> Song
    Playlist playlist;                           // Circular doubly linked list
    stack<Song> recentHistory;                   // Stack: recently played
    queue<Song> upcomingQueue;                   // Queue: play next
    priority_queue<Song, vector<Song>, ComparePlayCount> mostPlayed;
    string currentSongId;
    bool isPlaying;

public:
    MusicPlayer() : isPlaying(false) {
        // Seed library
        addToLibrary("S001", "Bohemian Rhapsody", "Queen", "A Night at the Opera", 354);
        addToLibrary("S002", "Stairway to Heaven", "Led Zeppelin", "Led Zeppelin IV", 482);
        addToLibrary("S003", "Hotel California", "Eagles", "Hotel California", 391);
        addToLibrary("S004", "Imagine", "John Lennon", "Imagine", 187);
        addToLibrary("S005", "Smells Like Teen Spirit", "Nirvana", "Nevermind", 301);
    }

    // -------------------------------------------------------------------
    // ADD TO LIBRARY: Hash table insertion. O(1) avg.
    // -------------------------------------------------------------------
    void addToLibrary(string id, string title, string artist,
                      string album, int duration) {
        if (library.find(id) != library.end()) {
            cout << "  [ERROR] Song " << id << " already in library.\n";
            return;
        }
        library[id] = Song(id, title, artist, album, duration);
        cout << "  [OK] '" << title << "' added to library.\n";
    }

    // -------------------------------------------------------------------
    // ADD TO PLAYLIST: From library to circular linked list. O(1).
    // -------------------------------------------------------------------
    void addToPlaylist(string songId) {
        auto it = library.find(songId);
        if (it == library.end()) {
            cout << "  [ERROR] Song not found in library.\n";
            return;
        }
        playlist.addSong(it->second);
        cout << "  [OK] '" << it->second.title << "' added to playlist.\n";
    }

    // -------------------------------------------------------------------
    // PLAY: Start playing current song, push to history stack. O(1).
    // -------------------------------------------------------------------
    void play() {
        Song* s = playlist.getCurrent();
        if (!s) {
            // Try from upcoming queue
            if (!upcomingQueue.empty()) {
                Song nextSong = upcomingQueue.front();
                upcomingQueue.pop();
                // Search in library to add to playlist
                for (auto& pair : library) {
                    if (pair.second.songId == nextSong.songId) {
                        playlist.addSong(pair.second);
                        break;
                    }
                }
                s = playlist.getCurrent();
            }
            if (!s) {
                cout << "  [INFO] No songs to play. Add songs to playlist.\n";
                return;
            }
        }
        isPlaying = true;
        s->playCount++;
        recentHistory.push(*s);
        mostPlayed.push(*s);
        cout << "  [PLAY] '" << s->title << "' - " << s->artist
             << " [" << s->getDurationStr() << "]\n";
    }

    // -------------------------------------------------------------------
    // NEXT: Move to next song in circular list. O(1).
    // -------------------------------------------------------------------
    void nextTrack() {
        Song* s = playlist.next();
        if (s) {
            isPlaying = true;
            s->playCount++;
            recentHistory.push(*s);
            mostPlayed.push(*s);
            cout << "  [NEXT] '" << s->title << "' - " << s->artist << "\n";
        } else {
            cout << "  [INFO] End of playlist.\n";
            isPlaying = false;
        }
    }

    // -------------------------------------------------------------------
    // PREVIOUS: Move to previous song. O(1) via circular doubly linked list.
    // -------------------------------------------------------------------
    void prevTrack() {
        Song* s = playlist.prev();
        if (s) {
            isPlaying = true;
            cout << "  [PREV] '" << s->title << "' - " << s->artist << "\n";
        }
    }

    // -------------------------------------------------------------------
    // PLAY HISTORY: Stack - most recent first. O(n).
    // -------------------------------------------------------------------
    void showHistory() {
        if (recentHistory.empty()) {
            cout << "  [INFO] No play history.\n";
            return;
        }
        stack<Song> temp = recentHistory;
        cout << "  Recently Played (most recent first):\n";
        int i = 1;
        while (!temp.empty()) {
            Song s = temp.top();
            temp.pop();
            cout << "  " << i++ << ". " << s.title << " - " << s.artist << "\n";
        }
    }

    // -------------------------------------------------------------------
    // ADD TO UPCOMING QUEUE: FIFO - first added plays first. O(1).
    // -------------------------------------------------------------------
    void addToUpcoming(string songId) {
        auto it = library.find(songId);
        if (it == library.end()) {
            cout << "  [ERROR] Song not found.\n";
            return;
        }
        upcomingQueue.push(it->second);
        cout << "  [OK] '" << it->second.title << "' added to upcoming queue.\n";
    }

    // -------------------------------------------------------------------
    // SHOW UPCOMING: Display queue contents. O(n).
    // -------------------------------------------------------------------
    void showUpcoming() {
        if (upcomingQueue.empty()) {
            cout << "  [INFO] No upcoming songs.\n";
            return;
        }
        queue<Song> temp = upcomingQueue;
        cout << "  Upcoming Queue:\n";
        int i = 1;
        while (!temp.empty()) {
            Song s = temp.front();
            temp.pop();
            cout << "  " << i++ << ". " << s.title << " - " << s.artist << "\n";
        }
    }

    // -------------------------------------------------------------------
    // SHOW MOST PLAYED: Max-heap priority queue. O(n log n) to display.
    // -------------------------------------------------------------------
    void showMostPlayed() {
        if (mostPlayed.empty()) {
            cout << "  [INFO] No play data yet.\n";
            return;
        }
        cout << "  Most Played Songs:\n";
        // Copy heap to display without destroying original
        priority_queue<Song, vector<Song>, ComparePlayCount> temp = mostPlayed;
        int i = 1;
        while (!temp.empty()) {
            Song s = temp.top();
            temp.pop();
            cout << "  " << i++ << ". " << s.title << " - " << s.artist
                 << " (" << s.playCount << " plays)\n";
        }
    }

    void run() {
        cout << "\n=============================================\n";
        cout << "   MUSIC PLAYLIST MANAGER\n";
        cout << "   Data Structures: Circular Doubly LL,\n";
        cout << "   Stack, Queue, Hash Table, Heap\n";
        cout << "=============================================\n\n";

        int choice, duration;
        string id, title, artist, album, songId;

        do {
            cout << "\n------ MENU ------\n";
            cout << "  [LIBRARY - Hash Table]\n";
            cout << "    1. Add Song to Library\n";
            cout << "  [PLAYLIST - Circular Doubly LL]\n";
            cout << "    2. Add Song to Playlist\n";
            cout << "    3. Remove Current Song\n";
            cout << "    4. Display Playlist\n";
            cout << "  [PLAYBACK]\n";
            cout << "    5. Play\n";
            cout << "    6. Next (Circular LL)\n";
            cout << "    7. Previous (Circular LL)\n";
            cout << "  [HISTORY - Stack]\n";
            cout << "    8. Recently Played (LIFO)\n";
            cout << "  [UPCOMING - Queue]\n";
            cout << "    9. Add to Upcoming Queue\n";
            cout << "   10. Show Upcoming Queue\n";
            cout << "  [MOST PLAYED - Heap]\n";
            cout << "   11. Most Played Songs\n";
            cout << "    0. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    cout << "  Song ID: "; getline(cin, id);
                    cout << "  Title: "; getline(cin, title);
                    cout << "  Artist: "; getline(cin, artist);
                    cout << "  Album: "; getline(cin, album);
                    cout << "  Duration (sec): "; cin >> duration; cin.ignore();
                    addToLibrary(id, title, artist, album, duration);
                    break;
                case 2:
                    cout << "  Song ID: "; getline(cin, songId);
                    addToPlaylist(songId);
                    break;
                case 3:
                    if (playlist.removeCurrent())
                        cout << "  [OK] Current song removed.\n";
                    else
                        cout << "  [INFO] Playlist is empty.\n";
                    break;
                case 4: playlist.display(); break;
                case 5: play(); break;
                case 6: nextTrack(); break;
                case 7: prevTrack(); break;
                case 8: showHistory(); break;
                case 9:
                    cout << "  Song ID: "; getline(cin, songId);
                    addToUpcoming(songId);
                    break;
                case 10: showUpcoming(); break;
                case 11: showMostPlayed(); break;
                case 0: cout << "  Exiting. Goodbye!\n"; break;
                default: cout << "  [ERROR] Invalid choice.\n";
            }
        } while (choice != 0);
    }
};

int main() {
    MusicPlayer player;
    player.run();
    return 0;
}
