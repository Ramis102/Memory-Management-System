#include <iostream>
#include <string>
using namespace std;

class Block;
class Program;
class MemoryManagementSystem;

class Block
{
public:
	string ID;
	int totalBytes;
	Block* next;
public:
	Block()
	{
		ID = "NULL";
		totalBytes = 0;
		next = nullptr;
	}
};

template <typename T>
class LinkedList
{
private:

public:

};

template<>
class LinkedList<Block>
{
private:
	Block* head;
	Block* tail;
	int totalBlockBytes;
	int size;
public:
	LinkedList<Block>()
	{
		head = nullptr;
		tail = nullptr;
		size = 0;
		totalBlockBytes = 0;
	}
	Block* GetHead()
	{
		return head;
	}
	void AddEnd(int bytes, string id = "NULL")
	{
		Block* newBlock = new Block;
		newBlock->totalBytes = bytes;

		if (id == "NULL")
			newBlock->ID = to_string(size);
		else
			newBlock->ID = id;

		totalBlockBytes = totalBlockBytes + bytes;

		if (!tail)
		{
			head = newBlock;
			tail = newBlock;
		}
		else
		{
			tail->next = newBlock;
			tail = newBlock;
		}
		size++;
	}
	Block* MemoryAvailable(int bytes, bool strategy)
	{
		Block* availableBlock = nullptr;

		if (bytes > totalBlockBytes)
			return availableBlock;
		else
		{
			bool memFound = false;
			Block* temp = head;
			if (strategy == 0)
			{
				while (temp && !memFound)
				{
					if (temp->totalBytes >= bytes)
					{
						memFound = true;
						availableBlock = temp;
					}
					temp = temp->next;
				}
			}
			else
			{
				int diff = 0;
				bool check = false;
				while (temp)
				{
					if (check)
					{
						if ((diff > temp->totalBytes - bytes) && (temp->totalBytes - bytes >= 0))
						{
							diff = temp->totalBytes - bytes;
							availableBlock = temp;
						}
					}

					if (temp->totalBytes >= bytes && !check)
					{
						diff = temp->totalBytes - bytes;
						availableBlock = temp;
						check = true;
					}
					temp = temp->next;
				}
			}
			if (availableBlock)
			{
				availableBlock->totalBytes = availableBlock->totalBytes - bytes;
				totalBlockBytes = totalBlockBytes - bytes;
			}
			return availableBlock;
		}
	}
	void DeallocateList()
	{
		Block* curr = head;
		while (curr)
		{
			Block* temp = curr;
			curr = curr->next;
			delete temp;
		}
		head = nullptr;
		tail = nullptr;
		size = 0;
	}
	void Deallocate(Block* poolHead)
	{
		if (head)
		{
			Block* temp = head;	//head of block list of programs
			while (temp)
			{
				Block* temp2 = poolHead;
				bool found = false;
				while (temp2 && !found)
				{
					if (temp2->ID == temp->ID)
					{
						temp2->totalBytes = temp2->totalBytes + temp->totalBytes;
						found = true;
					}
					temp2 = temp2->next;
				}
				Block* curr = temp;
				temp = temp->next;
				delete curr;
				size--;
			}
			head = nullptr;
		}
	}
	void PrintList()
	{
		Block* temp = head;
		while (temp)
		{
			cout << "[ID: " << temp->ID << " Bytes: " << temp->totalBytes << "]";
			if (temp->next)
				cout << "->";
			temp = temp->next;
		}
		cout << endl;
	}
	void UpdateSize(int bytes)
	{
		totalBlockBytes = totalBlockBytes - bytes;
	}
	int GetTotalBytes()
	{
		return totalBlockBytes;
	}
};

class Program
{
public:
	string ID;
	int size;
	LinkedList<Block> progBlocks;
	Program* next;
public:
	Program()
	{
		ID = "NUll";
		size = 0;
		next = nullptr;
	}
	void UpdateBytes(int bytes)
	{
		size = size + bytes;
	}
};

template <>
class LinkedList<Program>
{
private:
	Program* head;
	Program* tail;
	int size;
public:
	LinkedList<Program>()
	{
		head = nullptr;
		tail = nullptr;
		size = 0;
	}
	void PrintList()
	{
		cout << "Programs:\n";
		Program* temp = head;
		while (temp)
		{
			cout << temp->ID << endl;
			cout << "Size: " << temp->size << endl;
			temp->progBlocks.PrintList();
			cout << endl;
			temp = temp->next;
		}
		cout << endl;
	}
	void AddEnd(int bytes, string id)
	{
		Program* newProg = new Program;
		newProg->size = bytes;
		newProg->ID = id;

		if (!tail)
		{
			head = newProg;
			tail = newProg;
		}
		else
		{
			tail->next = newProg;
			tail = newProg;
		}
		size++;
	}
	Program* SearchID(string id)
	{
		Program* prog = nullptr;
		bool found = false;
		Program* temp = head;

		while (temp && !found)
		{
			if (temp->ID == id)
			{
				found = true;
				prog = temp;
			}
			temp = temp->next;
		}

		return prog;
	}
	void AddForced(Program* prog, LinkedList<Block> pool, int bytes)
	{
		Block* poolHead = pool.GetHead();
		while (poolHead && bytes)
		{
			if (poolHead->totalBytes > 0)
			{
				if (poolHead->totalBytes < bytes)
				{
					AllocateBlock(prog, poolHead, poolHead->totalBytes);
					pool.UpdateSize(poolHead->totalBytes);
					bytes = bytes - poolHead->totalBytes;
					poolHead->totalBytes = 0;
				}
				else
				{
					AllocateBlock(prog, poolHead, bytes);
					pool.UpdateSize(bytes);
					poolHead->totalBytes = poolHead->totalBytes - bytes;
					bytes = 0;
				}

			}
			poolHead = poolHead->next;
		}
	}
	bool DeleteProgram(string id, Block* poolHead, Program* p = nullptr)
	{
		Program* prog;
		if (!p)
			prog = SearchID(id);
		else
			prog = p;

		if (prog)
		{
			prog->progBlocks.Deallocate(poolHead);
			if (prog == head)
			{
				Program* temp = head;
				head = head->next;
				delete temp;
			}
			else
			{
				Program* temp = head;
				while (temp->next != prog)
				{
					temp = temp->next;
				}
				temp->next = prog->next;
				if (prog == tail)
				{
					tail = temp;
				}
				delete prog;
			}
			size--;
			return true;
		}
		else
			return false;
	}
	void DeallocateList(Block* poolHead)
	{
		Program* currProg = head;
		while (currProg)
		{
			Program* temp = currProg;
			currProg = currProg->next;
			DeleteProgram(temp->ID, poolHead, temp);
		}
		head = nullptr;
		tail = nullptr;
		size = 0;
	}
	Program* GetEnd()
	{
		return tail;
	}
	void AllocateBlock(Program* prog, Block* block, int occupiedBytes)
	{
		prog->progBlocks.AddEnd(occupiedBytes, block->ID);
	}
};

class MemoryManagementSystem
{
private:
	LinkedList<Block> Pool;
	LinkedList<Program> Progs;
	int sizeOfMemory;
	bool strategy;
public:
	MemoryManagementSystem()
	{
		sizeOfMemory = 0;
		strategy = 1;
	}
	~MemoryManagementSystem()
	{
		Progs.DeallocateList(Pool.GetHead());
		Pool.DeallocateList();
	}
	void AllocatePool()
	{
		int blocks, blockBytes;
		cout << "Enter the number of memory blocks needed:\t";
		cin >> blocks;

		for (int i = 0; i < blocks; i++)
		{
			cout << "Enter block " << i + 1 << " bytes:\t";
			cin >> blockBytes;

			Pool.AddEnd(blockBytes);
		}
		cout << endl;
	}
	bool GetMem(string id, int memSize)
	{
		Block* availableBlock = Pool.MemoryAvailable(memSize, strategy);
		if (availableBlock)
		{
			Program* prog = Progs.SearchID(id);
			if (prog)
			{
				prog->UpdateBytes(memSize);
				Progs.AllocateBlock(prog, availableBlock, memSize);
			}
			else
			{
				Progs.AddEnd(memSize, id);
				Progs.AllocateBlock(Progs.GetEnd(), availableBlock, memSize);
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	bool AllocateForced(int memSize, string id)
	{
		if (Pool.GetTotalBytes() >= memSize)
		{
			Program* prog = Progs.SearchID(id);
			if (prog)
			{
				prog->UpdateBytes(memSize);
				Progs.AddForced(prog, Pool, memSize);
			}
			else
			{
				Progs.AddEnd(memSize, id);
				Progs.AddForced(Progs.GetEnd(), Pool, memSize);
			}
			return true;
		}
		else
			return false;
	}
	void Start()
	{
		AllocatePool();
		string progId;
		int bytes;
		bool allocated = true, force = false;
		int start = 1;

		while (start)
		{
			cout << "Enter 1 to start program\nEnter 2 to delete a program\nEnter 3 to print program list\nEnter 4 to print pool\nEnter 0 to quit\n";
			cin >> start;
			system("cls");

			if (start != 0)
			{
				if (start == 1)
				{
					cout << "Enter Program ID:\t";
					cin >> progId;
					cout << "Enter Program size in bytes:\t";
					cin >> bytes;
					cout << "Enter 1 for best fit and 0 for first fit:\t";
					cin >> strategy;

					allocated = GetMem(progId, bytes);
					if (!allocated)
					{
						cout << "No memory available\n";
						cout << "Enter 1 for forced allocation and 0 for no allocation:\t";
						cin >> force;
						if (force)
						{
							if (AllocateForced(bytes, progId))
								cout << "Forced memory allocated\n";
							else
								cout << "Forced memory allocation unsuccessful\n";
						}
						else
						{
							cout << "Program not added\n";
						}
					}
					else
						cout << "Program added";
					cout << endl;
				}
				else if (start == 2)
				{
					string id;
					cout << "Enter program name:\t";
					cin >> id;
					if (Progs.DeleteProgram(id, Pool.GetHead()))
						cout << "Program deleted\n";
					else
						cout << "Program does not exist\n";
				}
				else if (start == 3)
				{
					Progs.PrintList();
				}
				else
				{
					Pool.PrintList();
				}
				cout << endl;
			}

		}
	}
};

int main()
{
	MemoryManagementSystem system;
	system.Start();
	return 0;
}