
#include <stdexcept>
#include <memory>
using std::logic_error;
using std::out_of_range;
using std::runtime_error;

template <class Key>
class binomial_heap
{
	class Pointer;

	class tree_node
	{
	public:
		Key value; 
		tree_node* parent; 
		tree_node* sibling; 
		tree_node* child; 
		std::shared_ptr<Pointer> pointer;

		class tree_node(Key value)
		{
			this->child = nullptr;
			this->pointer = std::shared_ptr<Pointer>(new Pointer(this));
			this->sibling = nullptr;
			this->degree = 0;
			this->parent = nullptr;
			this->value = value;
		}

		tree_node* tree_merge(tree_node* new_tree) 
		{
			if (this->degree != new_tree->degree)
			{
				throw logic_error("Ó äåðåâüåâ ðàçíàÿ ñòåïåíü!");
			}
			tree_node* new_root = this;
			tree_node* new_sibling = new_tree;
			if (new_sibling->value < new_root->value)
			{
				std::swap(new_root, new_sibling);
			}
			new_root->add_child(new_sibling); //ïîäâåøèâàåì çà äåðåâî ñ áîëüøèì çíà÷åíèåì value
			return new_root;
		}

		unsigned long give_deg()
		{
			return degree;
		}

		void deleteDeg()
		{
			degree = 0;
		}

	private:
		unsigned long degree;
		void add_child(tree_node* new_sibling) // ïîäâåøèâàåì îäíî äåðåâî ê äðóãîìó
		{
			new_sibling->parent = this;
			new_sibling->sibling = this->child;
			this->degree += 1;
			this->child = new_sibling;
		}
	};

	class Pointer
	{
		friend class binomial_heap<Key>;
	public:

		Key value()
		{
			return node->value;
		}

		bool is_mark()
		{
			return mark;
		}

		Pointer(tree_node* node)
		{
			this->node = node;
			this->mark = true;
		}
	private:
		bool mark{};
		tree_node* node;
	};


public:
	std::shared_ptr<Pointer> insert(Key val)
	{
		auto new_node = new tree_node(val);
		add_roots(new_node);
		++heap_size;
		return new_node->pointer;
	}

	unsigned long size()
	{
		return heap_size;
	}

	bool is_empty()
	{
		return heap_size == 0;
	}

	Key get_min()
	{
		if (is_empty())
		{
			throw runtime_error("Êó÷à ïóñòàÿ!");
		}
		return min_root->value;
	}

	Key extract_min()
	{
		if (is_empty())
		{
			throw std::runtime_error("Êó÷à ïóñòàÿ!");
		}
		auto erased_root = extract_min_root();

		Key return_value = erased_root->value;
		delete erased_root;

		return return_value;
	}

	void change(std::shared_ptr<Pointer> pointer, Key new_value)
	{
		if (!pointer->is_mark())
		{
			throw std::runtime_error("Âåðøèíà óæå óäàëåíà!");
		}

		if (new_value <= pointer->value())
		{
			pointer->node->value = new_value;
			sift_up(pointer->node);
		}
		else
		{
			extract_value(pointer->node);
			//óçåë íàõîäèòñÿ âíå êó÷è, ïîýòîìó ìû ìîæåì èçìåíèòü åãî çíà÷åíèå
			pointer->node->value = new_value;
			add_roots(pointer->node);
			++heap_size;
		}
	}

	void erase(std::shared_ptr<Pointer> pointer)
	{
		if (!pointer->is_mark())
		{
			throw runtime_error("Âåðøèíà óæå óäàëåíà!");
		}
		auto node = pointer->node;
		extract_value(node);

		delete pointer->node;
		pointer->mark = false;
	}

	void merge(binomial_heap& second_heap)
	{
		if (second_heap.size() == 0)
		{
			return;
		}
		add_roots(second_heap.minimal_tree);

		if (min_root == nullptr ||
			second_heap.min_root != nullptr && second_heap.min_root->value < min_root->value)
		{
			min_root = second_heap.min_root;
		}
		heap_size += second_heap.heap_size;

		second_heap.min_root = second_heap.minimal_tree = nullptr;
		second_heap.heap_size = 0;
	}

	~binomial_heap()
	{
		recursive_delete(minimal_tree);
	}

private:
	unsigned long heap_size = 0;
	tree_node* minimal_tree;
	tree_node* min_root;

	void add_roots(tree_node* second_min_root)
	{
		tree_node* prev_root = nullptr;
		tree_node* current_root = minimal_tree;
		tree_node* free_root = second_min_root;
		second_min_root = second_min_root->sibling;

		while (free_root != nullptr)
		{
			if (second_min_root != nullptr && free_root->give_deg() == second_min_root->give_deg())
			{
				auto new_second_root = second_min_root->sibling;
				free_root = free_root->tree_merge(second_min_root);

				second_min_root = new_second_root;
			}
			else
			{
				if (current_root == nullptr || free_root->give_deg() < current_root->give_deg())
				{
					if (prev_root == nullptr)
					{
						minimal_tree = free_root;
					}
					else
					{
						prev_root->sibling = free_root;
					}
					free_root->sibling = current_root;
					update_min_if_needed(free_root);

					current_root = free_root;
					free_root = second_min_root;
					if (second_min_root != nullptr)
					{
						second_min_root = second_min_root->sibling;
					}
				}
				else if (free_root->give_deg() > current_root->give_deg())
				{
					prev_root = current_root;
					current_root = current_root->sibling;
				}
				else
				{
					auto new_current_root = current_root->sibling;
					cut_out_root(prev_root, current_root);
					free_root = free_root->tree_merge(current_root);

					current_root = new_current_root;
				}
			}
		}
	}

	void cut_out_root(tree_node* prev_root, tree_node* current_root)
	{
		if (prev_root != nullptr && prev_root->sibling != current_root)
		{
			throw runtime_error("Êîðíè îáðàçóþò íå îáðàçóþò ñïèñîê!");
		}

		if (prev_root != nullptr)
		{
			prev_root->sibling = current_root->sibling;
		}
		else
		{
			minimal_tree = current_root->sibling;
		}
		current_root->sibling = nullptr;
	}

	void update_min_if_needed(tree_node* root)
	{
		if (min_root == nullptr || root->value <= min_root->value)
		{
			min_root = root;
		}
	}

	void find_new_min()
	{
		for (auto root = minimal_tree; root != nullptr; root = root->sibling)
		{
			update_min_if_needed(root);
		}
	}

	tree_node* reverse_siblings(tree_node* first, bool recursive_call = false)
	{
		if (first->sibling == nullptr)
		{
			return first;
		}
		else
		{
			tree_node* new_first = reverse_siblings(first->sibling);
			first->sibling->sibling = first;
			if (!recursive_call)
			{
				first->sibling = nullptr;
			}

			return new_first;
		}
	}

	void swap_values(tree_node* first, tree_node* second)
	{
		std::swap(first->value, second->value);
		std::swap(first->pointer->node, second->pointer->node);
		std::swap(first->pointer, second->pointer);
	}

	void sift_up(tree_node* node, bool rise_to_top = false)
	{
		while (node->parent != nullptr && (node->value < node->parent->value || rise_to_top))
		{
			swap_values(node, node->parent);
			node = node->parent;
		}

		if (rise_to_top || node->value < min_root->value)
		{
			min_root = node;
		}
	}

	tree_node* extract_min_root()
	{
		tree_node* root_before_min = nullptr;
		if (minimal_tree != min_root)
		{
			root_before_min = minimal_tree;
			while (root_before_min->sibling != min_root)
			{
				root_before_min = root_before_min->sibling;
			}
		}

		cut_out_root(root_before_min, min_root);
		--heap_size;
		auto erased_root = min_root;
		min_root = nullptr;

		if (erased_root->child != nullptr)
		{
			auto minimal_tree_sibling = reverse_siblings(erased_root->child);
			erased_root->child = nullptr;
			erased_root->deleteDeg();
			auto current_node = minimal_tree_sibling;
			while (current_node != nullptr)
			{
				current_node->parent = nullptr;
				current_node = current_node->sibling;
			}

			add_roots(minimal_tree_sibling);
		}
		find_new_min();

		return erased_root;
	}

	void extract_value(tree_node* node)
	{
		//èçìåíÿåì çíà÷åíèå â íóæíîé âåðøèíå íà ìèíóñ áåñêîíå÷íîñòü, ïðîñåèâàåì è èçâëåêàåì åå
		sift_up(node, true);
		extract_min_root();
	}

	void recursive_delete(tree_node* node)
	{
		if (node == nullptr)
		{
			return;
		}
		recursive_delete(node->sibling);
		recursive_delete(node->child);

		delete node;
	}
};

