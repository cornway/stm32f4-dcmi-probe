#ifndef GBROWSER_H
#define GBROWSER_H
#include "graphic.h"
#include "printer.h"
#include "gComponent.h"

#define DefaultGBNodeCharCapacity 24


class GBNode :  public Link<GBNode>,
                public std_printer::Printer<char, '\0', '\n', '0', '-'>    {
    private :
        int8_t attribute;
        void *value;
    public :
        GBNode (char *name, int size) : std_printer::Printer<char, '\0', '\n', '0', '-'> (size)
        {
            this->setText(name);
            this->attribute = 0;
        }
        ~GBNode ()
        {
            
        }
        
        void setAttribute (int8_t attribute)
        {
            this->attribute = attribute;
        }
        
        void setValue (void *value)
        {
            this->value = value;
        }
        
        int8_t getAttribute ()
        {
            return this->attribute;
        }
        
        void *getValue ()
        {
            return this->value;
        }
        
}; /*class GBNode*/

template <typename Color, typename Range, Color white>
class GBrowser : public GComponent<Color, Range, white>,
                 public Link<GBrowser<Color, Range, white> >,
                 public std_printer::Printer<char, '\0', '\n', '0', '-'>  {
        private :
            Color headerColor;
            Color selectColor;
            vector::Vector<GBNode> nodes;
            GBNode *selectedNode;
            GBNode *windowStart;
            uint16_t nodesInWindow;
            Range nodeIndex;
            static const Range gSpacing = 4;

        public:

        
        GBrowser (Graphic<Color, Range, white> *graphic, int textSize) : GComponent<Color, Range, white> (graphic), 
                                                                         Link<GBrowser<Color, Range, white> > (),
                                                                         std_printer::Printer<char, '\0', '\n', '0', '-'> (textSize)
        {
            this->glow = 0;
            this->windowStart = nullptr;
            this->selectedNode = nullptr;
            this->nodeIndex = 0;
            this->headerColor = 0x0;
        }
        ~GBrowser ()
        {
            this->removeAllNodes();
        }   

        void refresh ()
        {
            this->windowStart = this->nodes.getFirst();
            this->selectedNode = this->windowStart;
            int hstep = this->getGraphic()->getFontHeight() + gSpacing;
            this->nodesInWindow = (this->h - hstep) / hstep;
            this->wakeUp();
        }
        
        GBNode *addNode (char *name, int size)
        {
            this->wakeUp();
            GBNode *node = new GBNode(name, size);
            if (node == nullptr) {
                return nullptr;
            }
            this->nodes.addFirst(node);
            return node;
        }
        
        int removeNode (char *name)
        {
            this->wakeUp();
            GBNode *node = this->nodes.getFirst();
            while (node != nullptr) {
                if (strcmp(node->getText(), name) == 0) {
                    this->nodes.remove(node);
                    return 0;
                }
                node = node->next();
            }
            return -1;
        }
        
        void removeNode (GBNode *node)
        {
            this->wakeUp();
            this->nodes.remove(node);
        }
        
        void removeAllNodes ()
        {
            this->wakeUp();
            GBNode *node = this->nodes.getFirst();
            GBNode *nnode = node->next();
            while (node != nullptr) {
                nodes.remove(node);
                delete node;
                node = nnode;
                nnode = nnode->next();
            }
        }
        
        void moveUp ()
        {
            this->wakeUp();
            this->selectedNode = this->selectedNode->next();
            if (this->selectedNode == nullptr) {
                this->nodeIndex = 0;
                this->selectedNode = this->nodes.getFirst();
                this->windowStart = this->selectedNode;
                return;
            }
            this->nodeIndex++;
            if (this->nodeIndex >= this->nodesInWindow) {
                this->nodeIndex = 0;
                this->windowStart = this->selectedNode;
                if (this->windowStart == nullptr) {
                    this->selectedNode = this->nodes.getFirst();
                    this->windowStart = this->selectedNode;
                    return;
                }
            }
        }
        
        void moveDown ()
        {
            this->wakeUp();
            this->selectedNode = this->selectedNode->prev();
            if (this->selectedNode == nullptr) {
                this->selectedNode = this->nodes.getFirst();
                this->windowStart = this->selectedNode;
                return;
            }
            this->nodeIndex--;
            if (this->nodeIndex < 0) {
                this->nodeIndex = this->nodesInWindow - 1;
                uint32_t index = this->nodesInWindow;
                this->windowStart = this->selectedNode;
                while (--index) {
                    this->windowStart = this->windowStart->prev();
                }
            }
        }
        
        virtual void repaint () final
        {
            Range hstep = this->graphic->getFontHeight() + this->gSpacing;
            Box<Range> headerBox = {this->x, this->y + this->h - hstep, this->w, hstep};
            this->graphic->fill(headerBox, this->headerColor);
            if (this->getTextSize() > 0) {
                this->graphic->drawString(headerBox, this->getText(), this->foreground);
            }
            this->graphic->fill(this->x, this->y, this->w, this->h - hstep, this->background);
            
            GBNode *node = this->windowStart;
            Box<Range> nodeBox = {this->x, headerBox.y - hstep, this->w, hstep};
            int index = 0;
            while ((node != nullptr) && (index++ < this->nodesInWindow)) {
                if (node == this->selectedNode) {
                    this->graphic->fill(nodeBox, this->selectColor);
                }
                this->graphic->drawString(nodeBox, node->getText(), this->foreground);
                nodeBox.y = nodeBox.y - hstep;
                node = node->next();
            }
        }
        
        void setHeaderColor (Color color)
        {
            this->wakeUp();
            this->headerColor = color;
        }
        
        void setSelectColor (Color color)
        {
            this->wakeUp();
            this->selectColor = color;
        }
        
        GBNode *getSelectedNode ()
        {
            this->wakeUp();
            return this->selectedNode;
        }
        
        Color getHeaderColor ()
        {
            this->wakeUp();
            return this->headerColor;
        }
        
};


#endif /*GBROWSER_H*/
